/*
 * Mailbox folder access
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
#include <buffy/mailfolder/mailbox.h>
#include <buffy/mailfolder/maildir.h>

using namespace std;
using namespace buffy;

std::shared_ptr<MailFolder> MailFolder::accessFolder(const std::string& path)
{
    std::shared_ptr<MailFolder> res = mailfolder::Mailbox::accessFolder(path);
    if (!res)
        res = mailfolder::Maildir::accessFolder(path);
    return res;
}


void MailFolder::enumerateFolders(const std::string& path, Consumer<std::shared_ptr<MailFolder>>& cons)
{
    mailfolder::Mailbox::enumerateFolders(path, cons);
    mailfolder::Maildir::enumerateFolders(path, cons);
}

class MailFolderCollector : public MailFolderConsumer, public std::vector<std::shared_ptr<MailFolder>>
{
public:
    virtual ~MailFolderCollector() throw () {}
    virtual void consume(std::shared_ptr<MailFolder> mf)
    {
        emplace_back(mf);
    }
};

std::vector<std::shared_ptr<MailFolder>> MailFolder::enumerateFolders(const std::string& path)
{
    MailFolderCollector res;
    enumerateFolders(path, res);
    return res;
}
