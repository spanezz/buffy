#ifndef SORTERFILTER_H
#define SORTERFILTER_H

#include <QSortFilterProxyModel>
#include "folders.h"

class SorterFilter : public QSortFilterProxyModel
{
    Q_OBJECT

protected:
    // View all folders, no matter what
    bool view_all;
    // View any folder as long as it is nonempty
    bool view_all_nonempty;
    // View any folder as long as it has flagged messages
    bool view_all_flagged;

    Folders& folders;

    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

public:
    explicit SorterFilter(Folders& folders, QObject *parent = 0);

    void update_visibility(bool refilter=true);

signals:

public slots:

};

#endif // SORTERFILTER_H
