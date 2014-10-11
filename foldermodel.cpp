#include "foldermodel.h"
#include <algorithm>
#include <QColor>
#include <QFont>
#include <QDebug>

using namespace buffy;

FolderModel::FolderModel(Folders& folders, QObject *parent) :
    QAbstractTableModel(parent), folders(folders)
{
}

FolderModel::ColumnType FolderModel::resolveColumnType(int column) const
{
    switch (column)
    {
    case 0: return CT_NAME;
    case 1: return CT_NEW;
    case 2: return CT_UNREAD;
    case 3: return CT_TOTAL;
    case 4: return CT_FLAGGED;
    default: return CT_INVALID;
    }
}

int FolderModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return folders.all.size();
}

int FolderModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return 5;
}

QVariant FolderModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if ((unsigned)index.row() >= folders.all.size()) return QVariant();
    ColumnType ctype = resolveColumnType(index.column());
    if (ctype == CT_INVALID) return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        const Folder& f = folders.all[index.row()];
        switch (ctype)
        {
        case CT_NAME: return QVariant(f.folder.name().c_str());
        case CT_NEW: return QVariant(f.folder.getMsgNew());
        case CT_UNREAD: return QVariant(f.folder.getMsgUnread());
        case CT_TOTAL: return QVariant(f.folder.getMsgTotal());
        case CT_FLAGGED: return QVariant(f.folder.getMsgFlagged());
        default: return QVariant();
        }
        break;
    }
    case Qt::ToolTipRole:
    case Qt::StatusTipRole:
    {
        const Folder& f = folders.all[index.row()];
        return QVariant(f.folder.path().c_str());
        break;
    }
    case Qt::TextAlignmentRole:
        switch (ctype)
        {
        case CT_NAME: return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case CT_NEW:
        case CT_UNREAD:
        case CT_TOTAL:
        case CT_FLAGGED: return QVariant(Qt::AlignRight | Qt::AlignVCenter);
        default: return QVariant();
        }
        break;
    case Qt::BackgroundRole:
        switch (ctype)
        {
        case CT_NAME: return QColor(0xEEEEEE);
        default: return QVariant();
        }
        break;
    case Qt::FontRole:
        const Folder& f = folders.all[index.row()];
        if (f.cfg.getBool("activeinbox") && f.folder.getMsgNew())
        {
            QFont boldFont;
            boldFont.setBold(true);
            return boldFont;
        }
        break;
    }
    /*
    case Qt::SizeHintRole:
        break;
        */
    return QVariant();
}

QVariant FolderModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation != Qt::Horizontal)
        return QVariant();

    switch (resolveColumnType(section))
    {
    case CT_NAME: return QVariant("Name");
    case CT_NEW: return QVariant("New");
    case CT_UNREAD: return QVariant("Unread");
    case CT_TOTAL: return QVariant("Total");
    case CT_FLAGGED: return QVariant("Flagged");
    default: return QVariant();
    }
}

Qt::ItemFlags FolderModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    if ((unsigned)index.row() >= folders.all.size()) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

const Folder* FolderModel::valueAt(const QModelIndex &index) const
{
    if (!index.isValid()) return NULL;
    if ((unsigned)index.row() >= folders.all.size()) return NULL;
    return &folders.all[index.row()];
}

void FolderModel::reread_folders()
{
    beginResetModel();
    folders.rescan();
    endResetModel();
}
