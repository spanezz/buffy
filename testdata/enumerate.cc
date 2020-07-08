/*
 * Enumerate folders in one directory
 *
 * Copyright (C) 2005--2008  Enrico Zini <enrico@debian.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <wibble/exception.h>
#include <buffy/mailfolder.h>
#include <iostream>

using namespace std;
using namespace buffy;

int main(int argc, const char* argv[])
{
	if (argc == 1)
	{
		cout << "Usage: " << argv[0] << " directories" << endl
			 << endl
			 << "Enumerates all mail folders in the directories given." << endl;
		return 1;
	}

	try {
		for (int i = 1; i < argc; ++i)
		{
			cout << "Directory " << argv[i] << ":" << endl;
			std::vector<MailFolder> folders = MailFolder::enumerateFolders(argv[i]);
			for (std::vector<MailFolder>::const_iterator i = folders.begin();
					i != folders.end(); ++i)
			{
				cout << "  " << i->name() << ": [" << i->type() << "] " << i->path() << endl;
				/*
				i->updateStatistics();
				gen_ensure(empty.getMsgTotal() == 0);
				gen_ensure(empty.getMsgUnread() == 0);
				gen_ensure(empty.getMsgNew() == 0);
				gen_ensure(empty.getMsgFlagged() == 0);
				gen_ensure(empty.changed() == false);
				*/
			}
		}
		return 0;
	} catch (std::exception& e) {
		cerr << e.what() << endl;
		return 1;
	}
}

// vim:set ts=4 sw=4:
