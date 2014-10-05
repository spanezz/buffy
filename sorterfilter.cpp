#include "sorterfilter.h"

SorterFilter::SorterFilter(Folders& folders, QObject *parent) :
    QSortFilterProxyModel(parent), folders(folders)
{
}
