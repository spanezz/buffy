#ifndef BUFFY_MAILDIRMAILFOLDER_H
#define BUFFY_MAILDIRMAILFOLDER_H

#include <buffy/mailfolder.h>
#include <string>

namespace buffy {
namespace mailfolder {

class Maildir : public MailFolder
{
protected:
    std::string _name;
    std::string _path;
    int _stat_total;
    int _stat_unread;
    int _stat_new;
    int _stat_flagged;
    bool _deleted;
    time_t _new_mtime;
    time_t _cur_mtime;
    
public:
    Maildir(const std::string& path) throw ();
    Maildir(const std::string& name, const std::string& path) throw ();
    virtual ~Maildir() throw () {}

    virtual const std::string& name() const throw () { return _name; }
    virtual const std::string& path() const throw () { return _path; }
    virtual std::string type() const throw () { return "Maildir"; }

    virtual int getMsgTotal() const throw () { return _stat_total; }
    virtual int getMsgUnread() const throw () { return _stat_unread; }
    virtual int getMsgNew() const throw () { return _stat_new; }
    virtual int getMsgFlagged() const throw () { return _stat_flagged; }

    virtual bool changed();
    virtual void updateStatistics();

    static std::shared_ptr<MailFolder> accessFolder(const std::string& path);
    static void enumerateFolders(const std::string& dir, MailFolderConsumer& cons);
};

}
}

#endif
