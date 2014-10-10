#include "buffy.h"
#include "ui_buffy.h"
#include <QDebug>
#include <QResizeEvent>
#include <QMoveEvent>

using namespace buffy;

Buffy::Buffy(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Buffy),
    update_timer(this),
    folders_model(folders),
    sorterfilter(folders)
{
    //setStyleSheet("QHeaderView::section:horizontal {margin-left: 0; margin-right:0;}");
    //setStyleSheet("QHeaderView::section:horizontal {margin-right: 0;}");
    //setStyleSheet("QHeaderView::section:horizontal {margin-left:0; margin-right:0; margin-top:1px margin-bottom:1px}");
    setStyleSheet("QHeaderView::section:horizontal {margin-top: 3px; margin-bottom: 3px; margin-left: 0; margin-right: 0; }");

    ui->setupUi(this);
    sorterfilter.setSourceModel(&folders_model);
    sorterfilter.setDynamicSortFilter(true);
    ui->folders->setModel(&sorterfilter);
    auto header = ui->folders->horizontalHeader();
    header->setResizeMode(QHeaderView::ResizeToContents);
    header->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->folders->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    connect(ui->action_quit, SIGNAL(triggered()), this, SLOT(do_quit()));
    connect(ui->action_refresh, SIGNAL(triggered()), this, SLOT(do_refresh()));
    connect(ui->action_rescan, SIGNAL(triggered()), this, SLOT(do_rescan()));
    connect(header, SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(sort_changed(int, Qt::SortOrder)));
    connect(ui->folders, SIGNAL(activated(const QModelIndex&)), this, SLOT(folder_activated(const QModelIndex&)));
    connect(&update_timer, SIGNAL(timeout()), this, SLOT(do_refresh()));

    folders_model.reread_folders();

    config::Section prefs(folders.config.application("buffy"));
    if (prefs.getBool("sorted"))
    {
        int col = prefs.getInt("sort_col_id") - 1;
        Qt::SortOrder order = prefs.getInt("sort_order") == 0 ? Qt::AscendingOrder : Qt::DescendingOrder;

        //sorterfilter.sort(col, order);
        header->setSortIndicator(col, order);
    }

    QSize default_size = size();
    prefs.addDefault("winw", QString::number(default_size.width()).toStdString());
    prefs.addDefault("winh", QString::number(default_size.height()).toStdString());

    if (prefs.isSet("winx") && prefs.isSet("winy"))
    {
        setGeometry(prefs.getInt("winx"), prefs.getInt("winy"), prefs.getInt("winw"), prefs.getInt("winh"));
    } else {
        int saved_w = prefs.getInt("winw");
        int saved_h = prefs.getInt("winh");
        resize(QSize(saved_w, saved_h));
    }
}

Buffy::~Buffy()
{
    delete ui;
}

void Buffy::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    config::Section prefs(folders.config.application("buffy"));
    prefs.setInt("winw", event->size().width());
    prefs.setInt("winh", event->size().height());
}

void Buffy::moveEvent(QMoveEvent *event)
{
    QMainWindow::moveEvent(event);
    config::Section prefs(folders.config.application("buffy"));
    prefs.setInt("winx", event->pos().x());
    prefs.setInt("winy", event->pos().y());
}

void Buffy::do_quit()
{
    save_config();
    close();
}

void Buffy::do_rescan()
{
    folders.rescan();
    sorterfilter.invalidate();
}

void Buffy::do_refresh()
{
    folders_model.reread_folders();
}

void Buffy::save_config()
{
    folders.config.save();
}

void Buffy::sort_changed(int logicalIndex, Qt::SortOrder order)
{
    config::Section prefs(folders.config.application("buffy"));
    prefs.setBool("sorted", true);
    prefs.setInt("sort_col_id", logicalIndex + 1);
    prefs.setInt("sort_order", order == Qt::AscendingOrder ? 0 : 1);
}

void Buffy::folder_activated(const QModelIndex &idx)
{
    QModelIndex source_idx = sorterfilter.mapToSource(idx);
    config::MailProgram m = folders.config.selectedMailProgram();
    const Folder* f = folders_model.valueAt(source_idx);
    if (!f) return;
    qDebug() << "Running " << m.command("gui").c_str();
    m.run(f->folder, "gui");
}


