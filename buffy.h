#ifndef BUFFY_H
#define BUFFY_H

#include <QMainWindow>
#include "folders.h"
#include "foldermodel.h"
#include "sorterfilter.h"

namespace Ui {
class Buffy;
}

class Buffy : public QMainWindow
{
    Q_OBJECT

public:
    Folders folders;
    FolderModel folders_model;
    SorterFilter sorterfilter;

    explicit Buffy(QWidget *parent = 0);
    ~Buffy();

private slots:
    void do_quit();
    void save_config();
    void sort_changed(int logicalIndex, Qt::SortOrder order);
    void folder_activated(const QModelIndex&);

private:
    Ui::Buffy *ui;
};

#endif // BUFFY_H
