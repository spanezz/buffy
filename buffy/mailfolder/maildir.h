#ifndef BUFFY_MAILDIRMAILFOLDER_H
#define BUFFY_MAILDIRMAILFOLDER_H

/*
 * Maildir mail folder access
 *
 * Copyright (C) 2004--2008  Enrico Zini <enrico@debian.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

#include <buffy/mailfolder.h>
#include <string>

namespace buffy {
namespace mailfolder {

class Maildir : public MailFolderImpl
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

    static MailFolder accessFolder(const std::string& path);
    static void enumerateFolders(const std::string& dir, MailFolderConsumer& cons);
};

}
}

// vim:set ts=4 sw=4:
#endif
