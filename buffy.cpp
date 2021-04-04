#include "buffy.h"
#include "ui_buffy.h"
#include <QDebug>
#include <QResizeEvent>
#include <QMoveEvent>

using namespace buffy;

Buffy::Buffy(QApplication& app, Folders& folders, QWidget *parent) :
    QMainWindow(parent),
    app(app),
    update_timer(this),
    folders(folders),
    sorterfilter(folders),
    ui(new Ui::Buffy)
{
    //setStyleSheet("QHeaderView::section:horizontal {margin-left: 0; margin-right:0;}");
    //setStyleSheet("QHeaderView::section:horizontal {margin-right: 0;}");
    //setStyleSheet("QHeaderView::section:horizontal {margin-left:0; margin-right:0; margin-top:1px margin-bottom:1px}");
    setStyleSheet("QHeaderView::section:horizontal {margin-top: 3px; margin-bottom: 3px; margin-left: 0; margin-right: 0; }");

    ui->setupUi(this);
    sorterfilter.setSourceModel(&folders);
    sorterfilter.setDynamicSortFilter(true);
    ui->folders->setModel(&sorterfilter);

    connect(ui->action_quit, SIGNAL(triggered()), this, SLOT(do_quit()));
    connect(ui->action_hide, SIGNAL(triggered()), this, SLOT(do_hide()));
    connect(ui->action_refresh, SIGNAL(triggered()), &folders, SLOT(refresh()));
    connect(ui->action_rescan, SIGNAL(triggered()), &folders, SLOT(rescan()));
    connect(ui->action_view_all, SIGNAL(triggered()), this, SLOT(do_visibility_change()));
    connect(ui->action_view_all_nonempty, SIGNAL(triggered()), this, SLOT(do_visibility_change()));
    connect(ui->action_view_all_flagged, SIGNAL(triggered()), this, SLOT(do_visibility_change()));
    connect(ui->action_view_only_active_inboxes, SIGNAL(triggered()), this, SLOT(do_visibility_change()));
    connect(ui->action_column_new, SIGNAL(triggered()), this, SLOT(do_column_visibility_change()));
    connect(ui->action_column_unread, SIGNAL(triggered()), this, SLOT(do_column_visibility_change()));
    connect(ui->action_column_total, SIGNAL(triggered()), this, SLOT(do_column_visibility_change()));
    connect(ui->action_column_flagged, SIGNAL(triggered()), this, SLOT(do_column_visibility_change()));

    connect(ui->folders->horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(sort_changed(int, Qt::SortOrder)));
    connect(ui->folders, SIGNAL(activated(const QModelIndex&)), this, SLOT(folder_activated(const QModelIndex&)));
    connect(&update_timer, SIGNAL(timeout()), &folders, SLOT(refresh()));

    config::View view = folders.config.view();
    view.addDefault("col_new", "true");
    view.addDefault("col_unread", "true");
    view.addDefault("col_total", "true");
    view.addDefault("col_flagged", "true");

    ui->action_column_new->setChecked(view.getBool("col_new"));
    ui->action_column_unread->setChecked(view.getBool("col_unread"));
    ui->action_column_total->setChecked(view.getBool("col_total"));
    ui->action_column_flagged->setChecked(view.getBool("col_flagged"));

    update_column_visibility();

    update_timer.start(folders.config.general().interval() * 1000);

    config::Section prefs(folders.config.application("buffy"));
    if (prefs.getBool("sorted"))
    {
        int col = prefs.getInt("sort_col_id") - 1;
        Qt::SortOrder order = prefs.getInt("sort_order") == 0 ? Qt::AscendingOrder : Qt::DescendingOrder;

        //sorterfilter.sort(col, order);
        ui->folders->horizontalHeader()->setSortIndicator(col, order);
    }

    ui->action_view_all->setChecked(folders.config.view().empty());
    ui->action_view_all_nonempty->setChecked(folders.config.view().read());
    ui->action_view_all_flagged->setChecked(folders.config.view().important());
    ui->action_view_only_active_inboxes->setChecked(folders.config.view().getBool("only_active_inboxes"));

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

QAction *Buffy::action_quit() { return ui->action_quit; }

void Buffy::closeEvent(QCloseEvent *event)
{
    config::Section prefs(folders.config.application("buffy"));
    prefs.setBool("hidden", true);
    hide();
    event->ignore();
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
    app.quit();
}

void Buffy::do_hide()
{
    config::Section prefs(folders.config.application("buffy"));
    prefs.setBool("hidden", true);
    hide();
}

void Buffy::do_show()
{
    config::Section prefs(folders.config.application("buffy"));
    show();
    activateWindow();
    prefs.setBool("hidden", false);
}

void Buffy::do_visibility_change()
{
    folders.set_visibility(
                ui->action_view_all->isChecked(),
                ui->action_view_all_nonempty->isChecked(),
                ui->action_view_all_flagged->isChecked(),
                ui->action_view_only_active_inboxes->isChecked());
}

void Buffy::update_column_visibility()
{
    config::View view = folders.config.view();

    ui->folders->setColumnHidden(1, !view.getBool("col_new"));
    ui->folders->setColumnHidden(2, !view.getBool("col_unread"));
    ui->folders->setColumnHidden(3, !view.getBool("col_total"));
    ui->folders->setColumnHidden(4, !view.getBool("col_flagged"));
}

void Buffy::do_column_visibility_change()
{
    config::View view = folders.config.view();

    view.setBool("col_new", ui->action_column_new->isChecked());
    view.setBool("col_unread", ui->action_column_unread->isChecked());
    view.setBool("col_total", ui->action_column_total->isChecked());
    view.setBool("col_flagged", ui->action_column_flagged->isChecked());

    update_column_visibility();
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
    Folder* f = folders.valueAt(source_idx);
    if (!f) return;
    f->run_email_program();
}
