#include "folderview.h"
#include "foldermodel.h"
#include "folders.h"
#include "sorterfilter.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QDebug>

using namespace buffy;


FolderView::FolderView(QWidget *parent) :
    QTableView(parent)
{
}

void FolderView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = indexAt(event->pos());

    SorterFilter* sorted_model = dynamic_cast<SorterFilter*>(model());
    if (!sorted_model) return;
    QModelIndex source_idx = sorted_model->mapToSource(index);

    FolderModel* folder_model = dynamic_cast<FolderModel*>(sorted_model->sourceModel());
    if (!folder_model) return;

    const Folder* f = folder_model->valueAt(source_idx);
    if (!f) return;

    config::Folder foldercfg = folder_model->folders.config.folder(f->folder);

    QMenu menu(this);
    menu.addAction(new ViewAlwaysAction(foldercfg, &menu));
    menu.addAction(new HideAlwaysAction(foldercfg, &menu));
    menu.exec(event->globalPos());

    sorted_model->update_visibility();
}
