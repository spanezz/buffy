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

private:
    Ui::Buffy *ui;
};

#endif // BUFFY_H
