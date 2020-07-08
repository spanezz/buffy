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

//void MailFolder::enumerateFoldersSwig(const char* path, Consumer<MailFolder>& cons)
//{
//  return MailFolder::enumerateFolders(path, cons);
//}

MailFolder MailFolder::accessFolder(const std::string& path)
{
    MailFolder res = mailfolder::Mailbox::accessFolder(path);
    if (!res)
        res = mailfolder::Maildir::accessFolder(path);
    return res;
}


void MailFolder::enumerateFolders(const std::string& path, Consumer<MailFolder>& cons)
{
//  fprintf(stderr, "enumerating mailboxes\n");
    mailfolder::Mailbox::enumerateFolders(path, cons);
//  fprintf(stderr, "enumerating maildirs\n");
    mailfolder::Maildir::enumerateFolders(path, cons);
//  fprintf(stderr, "enumerated\n");
}

class MailFolderCollector : public MailFolderConsumer, public vector<MailFolder>
{
public:
    virtual ~MailFolderCollector() throw () {}
    virtual void consume(MailFolder& mf)
    {
        push_back(mf);
    }
};

vector<MailFolder> MailFolder::enumerateFolders(const std::string& path)
{
    MailFolderCollector res;
    enumerateFolders(path, res);
    return res;
}

// vim:set ts=4 sw=4: