#include "folders.h"
#include <algorithm>
#include <QColor>
#include <QFont>
#include <QMimeData>
#include <QProcess>
#include <QDebug>
#include <sys/types.h>  // getpwuid, getuid
#include <unistd.h>     // getuid
#include <iostream>


using namespace buffy;

enum ColumnType {
    CT_INVALID,
    CT_NAME,
    CT_NEW,
    CT_UNREAD,
    CT_TOTAL,
    CT_FLAGGED,
};

static ColumnType resolveColumnType(int column)
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


Folder::Folder(Folders &folders, std::shared_ptr<MailFolder> folder, QObject *parent)
    : QObject(parent), folders(folders), folder(folder)
{
    cfg().addDefault("activeinbox", "false");
}

config::Folder Folder::cfg()
{
    return folders.config.folder(folder);
}

config::Folder Folder::cfg() const
{
    return folders.config.folder(folder);
}

bool Folder::is_visible() const
{
    // View all folders, no matter what
    bool view_all = folders.config.view().empty();
    if (view_all) return true;
    auto cfg = this->cfg();
    if (cfg.getBool("activeinbox")) return true;
    if (cfg.forceview()) return true;
    if (cfg.forcehide()) return false;
    time_t ts = cfg.hideuntil();
    if (ts != 0 && time(nullptr) < ts) return false;
    if (folders.config.view().getBool("only_active_inboxes")) return false;
    if (folder->getMsgUnread()) return true;
    // View any folder as long as it has flagged messages
    bool view_all_flagged = folders.config.view().important();
    if (view_all_flagged && folder->getMsgFlagged()) return true;
    // View any folder as long as it is nonempty
    bool view_all_nonempty = folders.config.view().read();
    if (view_all_nonempty && folder->getMsgTotal()) return true;
    return false;
}

std::string Folder::name() const
{
    std::string name = folder->name();
    if (name.substr(0, 7) == "Maildir")
    {
        if (name.size() == 7)
            return "#INBOX";
        else
            return name.substr(7);
    } else
        return name;
}

void Folder::set_active_inbox(bool value)
{
    cfg().setBool("activeinbox", value);
    emit folders.visibility_updated();
}

[[noreturn]] static inline void throw_system_error(const std::string& what)
{
    throw std::system_error(errno, std::system_category(), what);
}

void Folder::run_email_program()
{
    auto cmd = folders.config.general().mua();
    std::string::size_type p;
    while ((p = cmd.find("%p")) != std::string::npos)
        cmd.replace(p, 2, folder->path());

    // TODO: use '~' as working directory
    std::vector<std::string> argv;
    argv.push_back("/bin/sh");
    //argv.push_back("/bin/sh");
    argv.push_back("-c");
    argv.push_back(cmd);

    // I wonder what this does, as it's undocumented
    // Glib::spawn_async(".", argv, Glib::SpawnFlags(0), sigc::mem_fun(*this, &MailProgramImpl::on_exit));

    QStringList args;
    args << "-c" << QString::fromStdString(cmd);

    QProcess* process = new QProcess(this);
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_mua_finished(int, QProcess::ExitStatus)));
    process->start("/bin/sh", args);

#if 0
    pid_t child = fork();
    if (child == -1)
        throw_system_error("cannot fork child process");

    if (child == 0)
    {
        // Child code path
        try {
            if (execl("/bin/sh", "/bin/sh", "-c", cmd.c_str(), 0) == -1)
                throw_system_error("cannot exec command in child process");
            throw_system_error("exec returned instead of replacing child process");
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        _exit(0);
    }
#endif
}

void Folder::on_mua_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    folders.refresh();
}


Folders::Folders(QObject* parent)
    : QAbstractTableModel(parent)
{
    config.view().addDefault("only_active_inboxes", "false");
}

Folders::~Folders()
{
}

void Folders::rescan()
{
    beginResetModel();

    for (auto f: all)
        delete f;
    all.clear();

    for (auto loc: config.locations())
    {
        auto folders = MailFolder::enumerateFolders(loc);
        for (auto folder: folders)
        {
            if (folder->changed()) folder->updateStatistics();
            all.push_back(new Folder(*this, folder, this));
        }
    }

    endResetModel();
}

void Folders::refresh()
{
    for (unsigned i = 0; i < all.size(); ++i)
    {
        if (all[i]->folder->changed())
        {
            all[i]->folder->updateStatistics();
            emit dataChanged(createIndex(i, 0), createIndex(i, 5));
        }
    }
    emit visibility_updated();
}

bool Folders::has_active_new() const
{
    for (auto f: all)
        if (f->cfg().getBool("activeinbox") && f->folder->getMsgNew())
            return true;
    return false;
}

void Folders::set_visibility(bool view_all, bool view_all_nonempty, bool view_all_flagged, bool view_only_active_inboxes)
{
    config.view().setEmpty(view_all);
    config.view().setRead(view_all_nonempty);
    config.view().setImportant(view_all_flagged);
    config.view().setBool("only_active_inboxes", view_only_active_inboxes);
    emit visibility_updated();
}


int Folders::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return all.size();
}

int Folders::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return 5;
}

QVariant Folders::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if ((unsigned)index.row() >= all.size()) return QVariant();
    ColumnType ctype = resolveColumnType(index.column());
    if (ctype == CT_INVALID) return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        const Folder& f = *all[index.row()];
        switch (ctype)
        {
        case CT_NAME: return QVariant(f.name().c_str());
        case CT_NEW: return QVariant(f.folder->getMsgNew());
        case CT_UNREAD: return QVariant(f.folder->getMsgUnread());
        case CT_TOTAL: return QVariant(f.folder->getMsgTotal());
        case CT_FLAGGED: return QVariant(f.folder->getMsgFlagged());
        default: return QVariant();
        }
        break;
    }
    case Qt::ToolTipRole:
    case Qt::StatusTipRole:
    {
        const Folder& f = *all[index.row()];
        return QVariant(f.folder->path().c_str());
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
        const Folder& f = *all[index.row()];
        if (f.cfg().getBool("activeinbox") && f.folder->getMsgNew())
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

QVariant Folders::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags Folders::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    if ((unsigned)index.row() >= all.size()) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

QStringList Folders::mimeTypes() const
{
    QStringList types;
    types << "text/plain";
    return types;
}

QMimeData* Folders::mimeData(const QModelIndexList &indices) const
{
    QMimeData* mimeData = new QMimeData();
    QString data;

    if (indices.isEmpty())
        data = "";
    else
    {
        int row = indices.first().row();
        if (row < 0 || (unsigned)row >= all.size())
            data = "";
        else
            data = QString::fromStdString(all[row]->folder->path());
    }

    mimeData->setData("text/plain", data.toUtf8());
    return mimeData;
}

Folder* Folders::valueAt(const QModelIndex &index)
{
    if (!index.isValid()) return NULL;
    if ((unsigned)index.row() >= all.size()) return NULL;
    return all[index.row()];
}

void Folders::each_active_inbox(std::function<void (Folder &)> func)
{
    for (auto f: all)
    {
        if (!f->cfg().getBool("activeinbox")) continue;
        func(*f);
    }
}
