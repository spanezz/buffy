#include "foldermodel.h"
#include <algorithm>
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
    return visible_folders.size();
}

int FolderModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return 5;
}

QVariant FolderModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if ((unsigned)index.row() >= visible_folders.size()) return QVariant();
    ColumnType ctype = resolveColumnType(index.column());
    if (ctype == CT_INVALID) return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        MailFolder f = visible_folders[index.row()];
        switch (ctype)
        {
        case CT_NAME: return QVariant(f.name().c_str());
        case CT_NEW: return QVariant(f.getMsgNew());
        case CT_UNREAD: return QVariant(f.getMsgUnread());
        case CT_TOTAL: return QVariant(f.getMsgTotal());
        case CT_FLAGGED: return QVariant(f.getMsgFlagged());
        default: return QVariant();
        }
        break;
    }
    case Qt::ToolTipRole:
    case Qt::StatusTipRole:
    {
        MailFolder f = visible_folders[index.row()];
        return QVariant(f.path().c_str());
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
    }
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
    case CT_NEW: return QVariant("N");
    case CT_UNREAD: return QVariant("U");
    case CT_TOTAL: return QVariant("T");
    case CT_FLAGGED: return QVariant("F");
    default: return QVariant();
    }
}

Qt::ItemFlags FolderModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    if ((unsigned)index.row() >= visible_folders.size()) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

MailFolder FolderModel::valueAt(const QModelIndex &index) const
{
    if (!index.isValid()) return NULL;
    if ((unsigned)index.row() >= visible_folders.size()) return NULL;
    return visible_folders[index.row()];
}

/*
typedef std::function<bool(const MailFolder& a, const MailFolder& b)> sort_func;

static sort_func get_sort_function(FolderModel::ColumnType ct, Qt::SortOrder order)
{
    switch (order)
    {
    case Qt::AscendingOrder:
        switch (ct)
        {
        case FolderModel::CT_NAME: return [](const MailFolder& mf1, const MailFolder& mf2) { return mf1.path() < mf2.path(); };
        case FolderModel::CT_NEW: return [](const MailFolder& mf1, const MailFolder& mf2) { return mf1.getMsgNew() < mf2.getMsgNew(); };
        case FolderModel::CT_UNREAD: return [](const MailFolder& mf1, const MailFolder& mf2) { return mf1.getMsgUnread() < mf2.getMsgUnread(); };
        case FolderModel::CT_TOTAL: return [](const MailFolder& mf1, const MailFolder& mf2) { return mf1.getMsgTotal() < mf2.getMsgTotal(); };
        case FolderModel::CT_FLAGGED: return [](const MailFolder& mf1, const MailFolder& mf2) { return mf1.getMsgFlagged() < mf2.getMsgFlagged(); };
        }
        break;
    case Qt::DescendingOrder:
        switch (ct)
        {
        case FolderModel::CT_NAME: return [](const MailFolder& mf1, const MailFolder& mf2) { return mf1.path() > mf2.path(); };
        case FolderModel::CT_NEW: return [](const MailFolder& mf1, const MailFolder& mf2) { return mf1.getMsgNew() > mf2.getMsgNew(); };
        case FolderModel::CT_UNREAD: return [](const MailFolder& mf1, const MailFolder& mf2) { return mf1.getMsgUnread() > mf2.getMsgUnread(); };
        case FolderModel::CT_TOTAL: return [](const MailFolder& mf1, const MailFolder& mf2) { return mf1.getMsgTotal() > mf2.getMsgTotal(); };
        case FolderModel::CT_FLAGGED: return [](const MailFolder& mf1, const MailFolder& mf2) { return mf1.getMsgFlagged() > mf2.getMsgFlagged(); };
        }
        break;
    }


}

void FolderModel::sort(int column, Qt::SortOrder order)
{
    std::sort(visible_folders.begin(), visible_folders.end(), get_sort_function(resolveColumnType(column), order));
    reset();
}
*/

void FolderModel::reread_folders()
{
    all_folders.clear();

    folders.find_folders([&](MailFolder folder) {
        if (!folder) qDebug() << "WTF" << endl;
        config::Folder fc = folders.config.folder(folder);
        all_folders.emplace_back(Folder{ folder, fc.forceview(), fc.forcehide() });
    });

    update_folders();
}

void FolderModel::update_folders()
{
    visible_folders.clear();

    // View all folders, no matter what
    bool view_all = folders.config.view().empty();
    // View any folder as long as it is nonempty
    bool view_all_nonempty = folders.config.view().read();
    // View any folder as long as it has flagged messages
    bool view_all_flagged = folders.config.view().important();

    for (auto folder : all_folders)
    {
        if (folder.folder.changed()) folder.folder.updateStatistics();
        if (!folder.is_visible(view_all, view_all_nonempty, view_all_flagged)) continue;
        visible_folders.push_back(folder.folder);
    }

    reset();
}

bool Folder::is_visible(bool view_all, bool view_all_nonempty, bool view_all_flagged) const
{
    if (view_all) return true;
    if (always_hide) return false;
    if (folder.getMsgNew()) return true;
    if (view_all_flagged && folder.getMsgFlagged()) return true;
    if (view_all_nonempty && folder.getMsgTotal()) return true;
    return false;
}
