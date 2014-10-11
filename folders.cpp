#include "folders.h"

using namespace buffy;

Folders::Folders()
{
}

void Folders::consume(MailFolder &folder)
{
    if (folder.changed()) folder.updateStatistics();
    config::Folder fc = config.folder(folder);
    all.emplace_back(Folder{ folder, fc });
}

void Folders::rescan()
{
    all.clear();
    for (auto loc: config.locations())
        MailFolder::enumerateFolders(loc, *this);
}

void Folders::refresh()
{
    for (auto f: all)
        if (f.folder.changed()) f.folder.updateStatistics();
}

bool Folder::is_visible(bool view_all, bool view_all_nonempty, bool view_all_flagged) const
{
    if (view_all) return true;
    if (cfg.forceview()) return true;
    if (cfg.forcehide()) return false;
    if (folder.getMsgUnread()) return true;
    if (view_all_flagged && folder.getMsgFlagged()) return true;
    if (view_all_nonempty && folder.getMsgTotal()) return true;
    return false;
}
