#ifndef BUFFY_H
#define BUFFY_H

#include <gtkmm.h>
#include "backend/config.h"

// #include <QMainWindow>
// #include <QTimer>
// #include <QMenu>
// #include <QAction>
// #include "folders.h"
// #include "sorterfilter.h"

class Buffy : public Gtk::Window
{
#if 0
protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    void update_column_visibility();
#endif
    buffy::config::Config config;
    buffy::config::Section prefs;

    bool on_configure_event(GdkEventConfigure* c);
    void on_show();

public:
#if 0
    QApplication& app;
    QTimer update_timer;
    Folders& folders;
    SorterFilter sorterfilter;
#endif

    // explicit Buffy(QApplication& app, Folders& folders, QWidget *parent = 0);
    Buffy();
    ~Buffy();

#if 0
    QAction* action_quit();

public slots:
    void do_hide();
    void do_show();

private slots:
    void do_quit();
    void do_visibility_change();
    void do_column_visibility_change();
    void save_config();
    void sort_changed(int logicalIndex, Qt::SortOrder order);
    void folder_activated(const QModelIndex&);
#endif
};


#endif // BUFFY_H
