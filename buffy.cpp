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
    folders_model(folders),
    sorterfilter(folders),
    preferences(this),
    ui(new Ui::Buffy)
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
    connect(ui->action_hide, SIGNAL(triggered()), this, SLOT(do_hide()));
    connect(ui->action_refresh, SIGNAL(triggered()), this, SLOT(do_refresh()));
    connect(ui->action_rescan, SIGNAL(triggered()), this, SLOT(do_rescan()));
    connect(ui->action_view_all, SIGNAL(triggered()), this, SLOT(do_visibility_change()));
    connect(ui->action_view_all_nonempty, SIGNAL(triggered()), this, SLOT(do_visibility_change()));
    connect(ui->action_view_all_flagged, SIGNAL(triggered()), this, SLOT(do_visibility_change()));
    connect(ui->action_column_new, SIGNAL(triggered()), this, SLOT(do_column_visibility_change()));
    connect(ui->action_column_unread, SIGNAL(triggered()), this, SLOT(do_column_visibility_change()));
    connect(ui->action_column_total, SIGNAL(triggered()), this, SLOT(do_column_visibility_change()));
    connect(ui->action_column_flagged, SIGNAL(triggered()), this, SLOT(do_column_visibility_change()));
    connect(ui->action_preferences, SIGNAL(triggered()), this, SLOT(do_preferences()));
    connect(&tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(tray_activated(QSystemTrayIcon::ActivationReason)));

    connect(header, SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(sort_changed(int, Qt::SortOrder)));
    connect(ui->folders, SIGNAL(activated(const QModelIndex&)), this, SLOT(folder_activated(const QModelIndex&)));
    connect(&update_timer, SIGNAL(timeout()), this, SLOT(do_refresh()));

    folders_model.reread_folders();

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
        header->setSortIndicator(col, order);
    }

    ui->action_view_all->setChecked(folders.config.view().empty());
    ui->action_view_all_nonempty->setChecked(folders.config.view().read());
    ui->action_view_all_flagged->setChecked(folders.config.view().important());

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

    tray_menu.addAction(ui->action_quit);
    tray.setContextMenu(&tray_menu);
    tray.show();
}

Buffy::~Buffy()
{
    delete ui;
}

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

void Buffy::do_rescan()
{
    folders.rescan();
    sorterfilter.update_visibility();
}

void Buffy::do_refresh()
{
    folders_model.reread_folders();
}

void Buffy::do_visibility_change()
{
    folders.config.view().setEmpty(ui->action_view_all->isChecked());
    folders.config.view().setRead(ui->action_view_all_nonempty->isChecked());
    folders.config.view().setImportant(ui->action_view_all_flagged->isChecked());
    sorterfilter.update_visibility();
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

void Buffy::do_preferences()
{
    // Fill preferences
    preferences.from_config(folders.config);

    // Run the dialog
    if (preferences.exec() == QDialog::Rejected)
        return;

    // Read back things if accepted
    preferences.to_config(folders.config);

    // Rescan folders
    do_rescan();

    // Reset update interval
    update_timer.stop();
    update_timer.start(folders.config.general().interval() * 1000);
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

void Buffy::tray_activated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        //QSystemTrayIcon::Unknown	0	Unknown reason
        case QSystemTrayIcon::Context:
        {
            //The context menu for the system tray entry was requested
          /*  QMenu menu(this);
            menu.exec(tray.geometry().topLeft());*/
            break;
        }
        //QSystemTrayIcon::DoubleClick	2	The system tray entry was double clicked
        case QSystemTrayIcon::Trigger:
        {
            config::Section prefs(folders.config.application("buffy"));

            // The system tray entry was clicked
            if (isVisible())
            {
                hide();
                prefs.setBool("hidden", true);
            } else {
                show();
                prefs.setBool("hidden", false);
            }
            break;
        }
        //QSystemTrayIcon::MiddleClick	4
    }
}


