/*
 * buffy - Mailbox watcher
 *
 * Copyright (C) 2003--2008  Enrico Zini
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

#include <buffy/config/config.h>
#include <buffy/mailfolder.h>
#include <wibble/exception.h>

#include <vector>
#include <iostream>
#include <memory>

using namespace std;
using namespace buffy;
using namespace wibble;

static void printStats(const MailFolder& f)
{
	cout << f.name() << ": " << f.type() << "[" << f.path() << "]"
		 << " new " << f.getMsgNew()
		 << ", unread " << f.getMsgUnread()
		 << ", total " << f.getMsgTotal()
		 << ", flagged " << f.getMsgFlagged()
		 << endl;
}

class StatsPrinter : public MailFolderConsumer
{
public:
	void consume(MailFolder& f)
	{
		f.updateStatistics();
		printStats(f);
	}
};

int usage(ostream& out, int res = 0)
{
	out << "Usage: lib command [args]" << endl
		<< endl
		<< "Test libbuffy functionalities." << endl
		<< endl
		<< "Commands are:" << endl
		<< "  list <path(s)>  scan the given path and show all mail folders" << endl
		<< "                  that are found." << endl
		<< "  show <path(s)>  show details about the mail folder found" << endl
		<< "                  at the given path." << endl
		<< "  cfgdump [file]  dump all configuration options found in the" << endl
		<< "                  given file, defaulting to ~/.buffy" << endl
		<< endl;
	return res;
}


int main(int argc, char* argv[])
{
	try {
		if (argc == 1)
			return usage(cout, 0);
		string cmd = argv[1];
		if (cmd == "list")
		{
			StatsPrinter printer;
			for (int i = 2; i < argc; ++i)
				MailFolder::enumerateFolders(argv[i], printer);
		} else if (cmd == "show") {
			for (int i = 2; i < argc; ++i)
			{
				MailFolder f = MailFolder::accessFolder(argv[i]);
				if (!f.valid())
					cerr << argv[i] << " is not a mail folder." << endl;
				else
				{
					if (f.changed())
						f.updateStatistics();
					printStats(f);
				}
			}
		} else if (cmd == "cfgdump") {
			auto_ptr<config::Config> cfg;
			if (argc > 2)
				cfg.reset(new config::Config(argv[2]));
			else
				cfg.reset(new config::Config());

			cout << "General:" << endl;
			cout << "  interval: " << cfg->general().interval() << endl;

			cout << "View:" << endl;
			cout << "  empty: " << cfg->view().empty() << endl;
			cout << "  read: " << cfg->view().read() << endl;
			cout << "  important: " << cfg->view().important() << endl;

			cout << "Locations:" << endl;
			vector<string> locations = cfg->locations();
			for (vector<string>::const_iterator i = locations.begin();
					i != locations.end(); ++i)
				cout << "  " << *i << endl;

			cout << "Mail programs:" << endl;
			std::vector<string> programs = cfg->mailPrograms();
			for (vector<string>::const_iterator i = programs.begin();
					i != programs.end(); ++i)
			{
				config::MailProgram mp = cfg->mailProgram(*i);
				cout << "  " << *i << ": text:" << mp.command("text")
			             << " gui:" << mp.command("gui") << (mp.selected() ? " (selected)" : "") << endl;
			}
			config::MailProgram sel = cfg->selectedMailProgram();
			cout << "  sel: " << sel.name() << ": text:" << sel.command("text")
		             << " gui:" << sel.command("gui") << (sel.selected() ? " (selected)" : "") << endl;

			//cout << "Applications:" << endl;
			//cout << "Folders:" << endl;
		} else {
			cerr << cmd << " is not a valid command." << endl;
			return usage(cerr, 1);
		}

		return 0;
	}
	catch (wibble::exception::Generic& e)
	{
		cerr << e.type() << ": " << e.fullInfo() << endl;
		return 1;
	}
}

// vim:set ts=4 sw=4:
