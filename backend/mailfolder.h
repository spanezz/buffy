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

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace buffy {

class MailFolder
{
public:
    virtual ~MailFolder() {}

    virtual const std::string& name() const throw () = 0;
    virtual const std::string& path() const throw () = 0;
    virtual std::string type() const throw () = 0;

    virtual int getMsgTotal() const throw () = 0;
    virtual int getMsgUnread() const throw () = 0;
    virtual int getMsgNew() const throw () = 0;
    virtual int getMsgFlagged() const throw () = 0;

    virtual bool changed() = 0;

    virtual void updateStatistics() = 0;

    /**
     * Create a MailFolder object given a path, autodetecting the folder type.
     *
     * If no suitable folder type has been found or other problems happened, it
     * will return a null MailFolder object.
     */
    static std::shared_ptr<MailFolder> accessFolder(const std::string& path);

    /**
     * Scan a directory and pass to a Consumer all the folders that are found
     */
    static void enumerateFolders(const std::string& path, std::function<void(std::shared_ptr<MailFolder>)> cons);

    /**
     * Scan a directory and return the folder it contains
     */
    static std::vector<std::shared_ptr<MailFolder>> enumerateFolders(const std::string& path);
};

}

// vim:set ts=4 sw=4:
#endif
