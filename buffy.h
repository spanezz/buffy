#ifndef BUFFY_H
#define BUFFY_H

#include <QMainWindow>
#include <QTimer>
#include "folders.h"
#include "foldermodel.h"
#include "sorterfilter.h"

namespace Ui {
class Buffy;
}

class Buffy : public QMainWindow
{
    Q_OBJECT

protected:
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;

public:
    QTimer update_timer;
    Folders folders;
    FolderModel folders_model;
    SorterFilter sorterfilter;

    explicit Buffy(QWidget *parent = 0);
    ~Buffy();

private slots:
    void do_quit();
    void do_rescan();
    void do_refresh();
    void do_visibility_change();
    void save_config();
    void sort_changed(int logicalIndex, Qt::SortOrder order);
    void folder_activated(const QModelIndex&);

private:
    Ui::Buffy *ui;
};

#endif // BUFFY_H
