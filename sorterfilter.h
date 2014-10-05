#ifndef SORTERFILTER_H
#define SORTERFILTER_H

#include <QSortFilterProxyModel>
#include "folders.h"

class SorterFilter : public QSortFilterProxyModel
{
    Q_OBJECT

protected:
    Folders& folders;

public:
    explicit SorterFilter(Folders& folders, QObject *parent = 0);

signals:

public slots:

};

#endif // SORTERFILTER_H
