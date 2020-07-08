#ifndef BUFFY_CONFIG_CONFIG_H
#define BUFFY_CONFIG_CONFIG_H

/*
 * Copyright (C) 2004--2008  Enrico Zini <enrico@enricozini.org>
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

struct _GKeyFile;
typedef struct _GKeyFile GKeyFile;

namespace buffy {
struct MailFolder;

namespace config {

class Config;

class Section
{
protected:
    Config& cfg;
    std::string section;

    Section(Config& cfg, const std::string& section);

    std::string getval(GKeyFile* cfg, const std::string& name) const;

public:
    bool isSet(const std::string& name);
    void unset(const std::string& name);

    bool getBool(const std::string& name) const;
    void setBool(const std::string& name, bool val);
    int getInt(const std::string& name) const;
    void setInt(const std::string& name, int val);

    std::string def(const std::string& name) const;
    std::string get(const std::string& name) const;
    void set(const std::string& name, const std::string& val);

    void addDefault(const std::string& name, const std::string& val);

    friend class Config;
};

class View : public Section
{
protected:
    View(Config& cfg, const std::string& section);

public:
    bool empty() const;
    bool read() const;
    bool important() const;

    void setEmpty(bool val);
    void setRead(bool val);
    void setImportant(bool val);

    friend class Config;
};

class General : public Section
{
protected:
    General(Config& cfg, const std::string& section);

public:
    int interval() const;
    void setInterval(int val);

    friend class Config;
};

class Folder : public Section
{
protected:
    Folder(Config& cfg, const std::string& section);

public:
    bool forceview() const;
    bool forcehide() const;

    void setForceView(bool val);
    void setForceHide(bool val);

    friend class Config;
};


class Location : public Section
{
protected:
    Location(Config& cfg, const std::string& section);

public:
    bool skip() const;
    void setSkip(bool val);

    friend class Config;
};

class MailProgram : public Section
{
protected:
    std::string m_name;

    MailProgram(const std::string& name, Config& cfg, const std::string& section);

    void setSelected(bool val);

public:
    std::string name() const;

    std::string command(const std::string& type) const;
    void setCommand(const std::string& type, const std::string& val);

    bool selected() const;

    void run(const buffy::MailFolder& folder, const std::string& cmdtype = "term");

    friend class Config;
};

class Config
{
protected:
    // State directory
    std::string rcfile;

    // Config file contents
    GKeyFile* cfg;

    // Default config values
    GKeyFile* defaults;

    void init();

    /**
     * Return the list of section names starting with the given prefix,
     * merging names in defaults and cfg, and stripping the prefix.
     */
    std::vector<std::string> secnames(const std::string& prefix);

public:
    Config();
    Config(const std::string& fname);
    ~Config();
    
    void clear();
    void load(const std::string& file);
    void save();
    void save(const std::string& file);
    void dump();

    //
    // Accessors for specific nodes
    //

    View view();
    General general();

    Section application(const std::string& name);

    std::vector<std::string> mailPrograms();
    MailProgram mailProgram(const std::string& name);
    MailProgram selectedMailProgram();
    void selectMailProgram(const std::string& name);

    Folder folder(const buffy::MailFolder& folder);
    Folder folder(const std::string& folder);

    std::vector<std::string> locations();
    Location location(const std::string& location);

    friend class Section;
};

}
}

// vim:set ts=4 sw=4:
#endif
