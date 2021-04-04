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

class Buffy : public Gtk::Application
{
public:
    buffy::config::Config config;
    buffy::config::Section prefs;
    Glib::RefPtr<Gtk::Builder> m_refbuilder;

    Buffy();

protected:
    void on_startup() override;
    void on_activate() override;

private:
    void create_window();
    void on_window_hide(Gtk::Window* window);
    void on_action_quit();

    template<typename TYPE>
    Glib::RefPtr<TYPE> get_ui_component(const char* name);
};


class BuffyWindow : public Gtk::ApplicationWindow
{
    Buffy& buffy;
    Gtk::Box m_box;

#if 0
protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    void update_column_visibility();
#endif

    bool on_configure_event(GdkEventConfigure* c);
    void on_show();

    void on_quit();

public:
#if 0
    QApplication& app;
    QTimer update_timer;
    Folders& folders;
    SorterFilter sorterfilter;
#endif

    // explicit Buffy(QApplication& app, Folders& folders, QWidget *parent = 0);
    BuffyWindow(Buffy& buffy);
    ~BuffyWindow();

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
