#ifndef BUFFY_MAILFOLDER_H
#define BUFFY_MAILFOLDER_H

/*
 * Abstract interface to mail folders
 *
 * Copyright (C) 2003--2008  Enrico Zini <enrico@debian.org>
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

#include "buffy/utils/consumer.h"
#include "buffy/utils/smartpointer.h"
#include <string>
#include <vector>

namespace buffy {

class MailFolderImpl : public SmartPointerItem
{
public:
    virtual ~MailFolderImpl() {}

    virtual const std::string& name() const throw () = 0;
    virtual const std::string& path() const throw () = 0;
    virtual std::string type() const throw () = 0;

    virtual int getMsgTotal() const throw () = 0;
    virtual int getMsgUnread() const throw () = 0;
    virtual int getMsgNew() const throw () = 0;
    virtual int getMsgFlagged() const throw () = 0;

    virtual bool changed() = 0;

    virtual void updateStatistics() = 0;
};

class MailFolder : public SmartPointer<MailFolderImpl>
{
public:
    MailFolder() throw () : SmartPointer<MailFolderImpl>() {}
    MailFolder(const MailFolder& mf) throw () : SmartPointer<MailFolderImpl>(mf) {}
    MailFolder(MailFolderImpl* otherimpl) throw () : SmartPointer<MailFolderImpl>(otherimpl) {}

    /**
     * Return true if this MailFolder is not a null mail folder
     */
    bool valid() const throw () { return impl; }
    
    /**
     * Return the presentation name of this mail folder
     */
    const std::string& name() const throw () { return impl->name(); }

    /**
     * Return the path to this mail folder
     */
    const std::string& path() const throw () { return impl->path(); }

    /**
     * Return the name of the type of this mail folder.
     *
     * For example: "Maildir" or "Mailbox"
     */
    std::string type() const throw () { return impl->type(); }

    /**
     * Get the total number of messages
     */
    int getMsgTotal() const throw () { return impl->getMsgTotal(); }
    
    /**
     * Get the number of unread messages
     */
    int getMsgUnread() const throw () { return impl->getMsgUnread(); }

    /**
     * Get the number of new messages
     */
    int getMsgNew() const throw () { return impl->getMsgNew(); }

    /**
     * Get the number of messages flagged 'important'
     */
    int getMsgFlagged() const throw () { return impl->getMsgFlagged(); }

    /**
     * Return true if the folder has been changed since the last updateStatistics
     *
     * A folder is not scanned automatically when the MailFolder object is
     * created: at the beginning, all statistics will return -1 and changed()
     * will return true;
     */
    bool changed() { return impl->changed(); }

    /**
     * Rescan the folder to update its statistics
     */
    void updateStatistics()
    {
        impl->updateStatistics();
    }

    /**
     * Create a MailFolder object given a path, autodetecting the folder type.
     *
     * If no suitable folder type has been found or other problems happened, it
     * will return a null MailFolder object.
     */
    static MailFolder accessFolder(const std::string& path);

    /**
     * Scan a directory and pass to a Consumer all the folders that are found
     */
    static void enumerateFolders(const std::string& path, Consumer<MailFolder>& cons);

    /**
     * Scan a directory and return the folder it contains
     */
    static std::vector<MailFolder> enumerateFolders(const std::string& path);
};

typedef Consumer<MailFolder> MailFolderConsumer;
typedef Filter<MailFolder> MailFolderFilter;

}

// vim:set ts=4 sw=4:
#endif
