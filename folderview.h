#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include <QTableView>
#include "buffy/config.h"

class FolderView : public QTableView
{
    Q_OBJECT

protected:
    void contextMenuEvent(QContextMenuEvent *event);

public:
    explicit FolderView(QWidget *parent = 0);

signals:

public slots:

};

#endif // FOLDERVIEW_H
