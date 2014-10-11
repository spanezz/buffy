#ifndef FOLDERS_H
#define FOLDERS_H

#include <buffy/config/config.h>
#include <buffy/mailfolder.h>
#include <functional>

struct Folder
{
    buffy::MailFolder folder;
    buffy::config::Folder cfg;

    bool is_visible(bool view_all, bool view_all_nonempty, bool view_all_flagged) const;
};

class Folders: public buffy::Consumer<buffy::MailFolder>
{
protected:
    void consume(buffy::MailFolder& folder) override;

public:
    buffy::config::Config config;
    std::vector<Folder> all;

    Folders();

    /// Look for folders on disk
    void rescan();

    /// Update folder statistics
    void refresh();
};

#endif // FOLDERS_H
