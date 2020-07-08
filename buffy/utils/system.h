#ifndef BUFFY_UTILS_SYSTEM_H
#define BUFFY_UTILS_SYSTEM_H

/*
 * Commodity wrappers for system functions
 *
 * Copyright (C) 2003--2013  Enrico Zini <enrico@debian.org>
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

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <set>

namespace buffy {

class InodeSet : protected std::set<ino_t>
{
public:
    InodeSet() {}
    InodeSet(const InodeSet& is) : std::set<ino_t>(is) {}
    InodeSet(ino_t inode) { insert(inode); }
    InodeSet operator+(ino_t inode) { InodeSet res(*this); res.add(inode); return res; }
    InodeSet& operator+=(ino_t inode) { add(inode); return *this; }
    void add(ino_t inode) { insert(inode); }
    bool has(ino_t inode) { return find(inode) != end(); }
};

}

// vim:set ts=4 sw=4:
#endif
