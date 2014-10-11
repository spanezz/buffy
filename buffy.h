#ifndef BUFFY_H
#define BUFFY_H

#include <QMainWindow>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include "folders.h"
#include "foldermodel.h"
#include "sorterfilter.h"
#include "preferences.h"

namespace Ui {
class Buffy;
}

class Buffy : public QMainWindow
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    void update_column_visibility();

public:
    QApplication& app;
    QTimer update_timer;
    Folders& folders;
    FolderModel folders_model;
    SorterFilter sorterfilter;
    Preferences preferences;
    QSystemTrayIcon tray;
    QMenu tray_menu;

    explicit Buffy(QApplication& app, Folders& folders, QWidget *parent = 0);
    ~Buffy();

public slots:
    void do_hide();
    void do_show();
    void do_rescan();
    void do_refresh();

private slots:
    void do_quit();
    void do_visibility_change();
    void do_column_visibility_change();
    void do_preferences();
    void save_config();
    void sort_changed(int logicalIndex, Qt::SortOrder order);
    void folder_activated(const QModelIndex&);
    void tray_activated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::Buffy *ui;
};

class ActivateInboxAction: public QAction
{
    Q_OBJECT

public:
    Folders& folders;
    buffy::MailFolder folder;

    ActivateInboxAction(Folders& folders, buffy::MailFolder folder, QObject* parent);

protected slots:
    void on_trigger();
};

#endif // BUFFY_H
