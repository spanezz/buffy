#ifndef SORTERFILTER_H
#define SORTERFILTER_H

#include <QSortFilterProxyModel>
#include "folders.h"

class SorterFilter : public QSortFilterProxyModel
{
    Q_OBJECT

protected:
    Folders& folders;

    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

public:
    explicit SorterFilter(Folders& folders, QObject *parent = 0);

signals:

public slots:
    void visibility_updated();
};

#endif // SORTERFILTER_H
