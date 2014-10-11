#include "folders.h"
#include <QDebug>

using namespace buffy;

Folders::Folders(QObject* parent)
    : QObject(parent)
{
}

Folders::~Folders()
{
}

void Folders::rescan()
{
    all.clear();

    for (auto loc: config.locations())
    {
        auto folders = MailFolder::enumerateFolders(loc);
        for (auto folder: folders)
        {
            if (folder.changed()) folder.updateStatistics();
            config::Folder fc = config.folder(folder);
            all.emplace_back(Folder{ folder, fc });
        }
    }
}

void Folders::refresh()
{
    for (auto f: all)
        if (f.folder.changed()) f.folder.updateStatistics();
}

void Folders::run_email_program(MailFolder folder)
{
    config::MailProgram m = config.selectedMailProgram();
    qDebug() << "Running " << m.command("gui").c_str();
    m.run(folder, "gui");
}

bool Folder::is_visible(bool view_all, bool view_all_nonempty, bool view_all_flagged) const
{
    if (view_all) return true;
    if (cfg.getBool("activeinbox")) return true;
    if (cfg.forceview()) return true;
    if (cfg.forcehide()) return false;
    if (folder.getMsgUnread()) return true;
    if (view_all_flagged && folder.getMsgFlagged()) return true;
    if (view_all_nonempty && folder.getMsgTotal()) return true;
    return false;
}
