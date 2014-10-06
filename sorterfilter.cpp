#include "sorterfilter.h"
#include "foldermodel.h"

SorterFilter::SorterFilter(Folders& folders, QObject *parent) :
    QSortFilterProxyModel(parent), folders(folders)
{
    update_visibility();
}

void SorterFilter::refresh()
{
    folders.refresh();
    invalidate();
}

bool SorterFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    FolderModel* model = dynamic_cast<FolderModel*>(sourceModel());
    if (!model) return true;

    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    const Folder* f = model->valueAt(index);
    if (!f) return false;

    return f->is_visible(view_all, view_all_nonempty, view_all_flagged);
}

void SorterFilter::update_visibility(bool refilter)
{
    view_all = folders.config.view().empty();
    view_all_nonempty = folders.config.view().read();
    view_all_flagged = folders.config.view().important();
    if (refilter) invalidateFilter();
}
