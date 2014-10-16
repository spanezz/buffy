#include "sorterfilter.h"
#include <QDebug>

SorterFilter::SorterFilter(Folders& folders, QObject *parent) :
    QSortFilterProxyModel(parent), folders(folders)
{
    connect(&folders, SIGNAL(visibility_updated()), this, SLOT(visibility_updated()));
    invalidateFilter();
}

void SorterFilter::visibility_updated()
{
    invalidateFilter();
}

bool SorterFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Folders* model = dynamic_cast<Folders*>(sourceModel());
    if (!model) return true;

    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    const Folder* f = model->valueAt(index);
    if (!f) return false;

    return f->is_visible();
}
