#ifndef BUFFY_H
#define BUFFY_H

#include <QMainWindow>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include "folders.h"
#include "sorterfilter.h"

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
    SorterFilter sorterfilter;

    explicit Buffy(QApplication& app, Folders& folders, QWidget *parent = 0);
    ~Buffy();

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

private:
    Ui::Buffy *ui;
};


#endif // BUFFY_H
