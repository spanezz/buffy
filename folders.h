#ifndef FOLDERS_H
#define FOLDERS_H

#include <buffy/config/config.h>
#include <buffy/mailfolder.h>
#include <functional>
#include <QObject>

struct Folder
{
    buffy::MailFolder folder;
    buffy::config::Folder cfg;

    bool is_visible(bool view_all, bool view_all_nonempty, bool view_all_flagged) const;
};

class Folders: public QObject
{
    Q_OBJECT

public:
    buffy::config::Config config;
    std::vector<Folder> all;

    Folders(QObject* parent=0);
    ~Folders();

    /// Look for folders on disk
    void rescan();

    /// Update folder statistics
    void refresh();

public slots:
    void run_email_program(buffy::MailFolder folder);
};

#endif // FOLDERS_H
