#include "folderview.h"
#include "folders.h"
#include "sorterfilter.h"
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMenu>
#include <QDebug>

using namespace buffy;


FolderView::FolderView(QWidget *parent) :
    QTableView(parent)
{
    setDragEnabled(true);
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    auto header = horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

void FolderView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = indexAt(event->pos());

    SorterFilter* sorted_model = dynamic_cast<SorterFilter*>(model());
    if (!sorted_model) return;
    QModelIndex source_idx = sorted_model->mapToSource(index);

    Folders* folder_model = dynamic_cast<Folders*>(sorted_model->sourceModel());
    if (!folder_model) return;

    Folder* f = folder_model->valueAt(source_idx);
    if (!f) return;

    QMenu menu(this);
    menu.addAction(new ActiveInboxAction(*f, &menu));
    menu.addAction(new ViewAlwaysAction(*f, &menu));
    menu.addAction(new HideAlwaysAction(*f, &menu));
    menu.exec(event->globalPos());

    sorted_model->visibility_updated();
}
