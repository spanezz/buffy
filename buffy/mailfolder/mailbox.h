#ifndef BUFFY_MAILBOXMAILFOLDER_H
#define BUFFY_MAILBOXMAILFOLDER_H

#include <buffy/mailfolder.h>
#include <string>

namespace buffy {
namespace mailfolder {

class Mailbox : public MailFolder
{
protected:
    std::string _name;
    std::string _path;
    int _stat_total;
    int _stat_unread;
    int _stat_new;
    int _stat_flagged;
    int _mbox_mtime;
    int _mbox_size;
    int _deleted;
    
public:
    Mailbox(const std::string& path) throw ();
    Mailbox(const std::string& name, const std::string& path) throw ();
    virtual ~Mailbox() throw () {}

    virtual const std::string& name() const throw () { return _name; }
    virtual const std::string& path() const throw () { return _path; }
    virtual std::string type() const throw () { return "Mailbox"; }

    virtual int getMsgTotal() const throw () { return _stat_total; }
    virtual int getMsgUnread() const throw () { return _stat_unread; }
    virtual int getMsgNew() const throw () { return _stat_new; }
    virtual int getMsgFlagged() const throw () { return _stat_flagged; }

    virtual bool changed();
    virtual void updateStatistics();

    static std::shared_ptr<MailFolder> accessFolder(const std::string& path);
    static void enumerateFolders(const std::string& dir, std::function<void(std::shared_ptr<MailFolder>)> cons);
};

}
}

#endif
