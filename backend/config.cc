#include "config.h"
#include "mailfolder.h"
#include "utils/string.h"

#include <glib.h>

#include <sys/types.h>  // getpwuid, getuid
#include <pwd.h>        // getpwuid
#include <unistd.h>     // getuid

#include <set>
#include <vector>
#include <system_error>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <cstdio>

//#define TRACE

namespace buffy {
namespace config {

Section::Section(Config& cfg, const std::string& section)
    : cfg(cfg), section(section)  {}

bool Section::isSet(const std::string& name) const
{
    if (!g_key_file_has_group(cfg.cfg, section.c_str()))
        return false;
    if (!g_key_file_has_key(cfg.cfg, section.c_str(), name.c_str(), NULL))
        return false;
    return true;
}

void Section::unset(const std::string& name)
{
    g_key_file_remove_key(cfg.cfg, section.c_str(), name.c_str(), NULL);
}

bool Section::getBool(const std::string& name) const
{
    return get(name) == "true";
}

void Section::setBool(const std::string& name, bool val)
{
    if (val)
        set(name, "true");
    else if (def(name).empty())
        // Revert to default if default is empty string (false)
        unset(name);
    else
        // If default is something other than empty string, we cannot
        // set name to the empty string or it will fallback on a
        // different default
        g_key_file_set_value(cfg.cfg, section.c_str(), name.c_str(), "false");
}

int Section::getInt(const std::string& name) const
{
    return strtoul(get(name).c_str(), 0, 10);
}

void Section::setInt(const std::string& name, int val)
{
    set(name, std::to_string(val));
}

std::string Section::getval(GKeyFile* cfg, const std::string& name) const
{
    if (!g_key_file_has_group(cfg, section.c_str()))
        return std::string();
    if (!g_key_file_has_key(cfg, section.c_str(), name.c_str(), NULL))
        return std::string();
    return g_key_file_get_value(cfg, section.c_str(), name.c_str(), NULL);
}

std::string Section::def(const std::string& name) const
{
    return getval(cfg.defaults, name);
}

std::string Section::get(const std::string& name) const
{
    std::string res = getval(cfg.cfg, name);
    if (res.empty())
        return def(name);
    return res;
}

void Section::set(const std::string& name, const std::string& val)
{
    std::string def = this->def(name);
    if (def == val)
        g_key_file_remove_key(cfg.cfg, section.c_str(), name.c_str(), NULL);
    else
        g_key_file_set_value(cfg.cfg, section.c_str(), name.c_str(), val.c_str());
}

void Section::addDefault(const std::string& name, const std::string& val)
{
    g_key_file_set_value(cfg.defaults, section.c_str(), name.c_str(), val.c_str());
}


View::View(Config& cfg, const std::string& section)
    : Section(cfg, section)
{
}

bool View::empty() const { return getBool("empty"); }
bool View::read() const { return getBool("read"); }
bool View::important() const { return getBool("important"); }

void View::setEmpty(bool val) { setBool("empty", val); }
void View::setRead(bool val) { setBool("read", val); }
void View::setImportant(bool val) { setBool("important", val); }


General::General(Config& cfg, const std::string& section)
    : Section(cfg, section)
{
}

int General::interval() const { return getInt("interval"); }
void General::setInterval(int val) { setInt("interval", val); }
std::string General::mua() { return get("mua"); }


Folder::Folder(Config& cfg, const std::string& section)
    : Section(cfg, section)
{
}

bool Folder::forceview() const { return getBool("forceview"); }
bool Folder::forcehide() const { return getBool("forcehide"); }
time_t Folder::hideuntil() const
{
    if (!isSet("hideuntil"))
        return 0;
    return getInt("hideuntil");
}
void Folder::setForceView(bool val)
{
    if (val)
    {
        setBool("forceview", true);
        unset("forcehide");
    } else
        unset("forceview");
}
void Folder::setForceHide(bool val)
{
    if (val)
    {
        setBool("forcehide", true);
        unset("forceview");
    } else
        unset("forcehide");
}
void Folder::setHideUntil(time_t ts)
{
    if (ts == 0)
        unset("hideuntil");
    else
        setInt("hideuntil", ts);
}

Location::Location(Config& cfg, const std::string& section)
    : Section(cfg, section)
{
}

bool Location::skip() const { return getBool("skip"); }
void Location::setSkip(bool val) { return setBool("skip", val); }


Config::Config()
    : cfg(0), defaults(0)
{
    init();
    load(rcfile);
}

Config::Config(const std::string& fname)
    : cfg(0), defaults(0)
{
    init();
    load(fname);
}

Config::~Config()
{
    if (cfg) g_key_file_free(cfg);
    if (defaults) g_key_file_free(defaults);
}

void Config::init()
{
    cfg = g_key_file_new();
    if (!cfg) throw std::runtime_error("cannot allocate GKeyFile for config");
    defaults = g_key_file_new();
    if (!defaults) throw std::runtime_error("cannot allocate GKeyFile for defaults");

    struct passwd* udata = getpwuid(getuid());
    rcfile = udata->pw_dir;
    rcfile += "/.buffy";

    // Fill in defaults
    g_key_file_set_value(defaults, "general", "interval", "300");
    g_key_file_set_value(defaults, "general", "mua", "/usr/bin/x-terminal-emulator -e \"/usr/bin/neomutt -f '%p'\"");
    g_key_file_set_value(defaults, "view", "important", "true");
    g_key_file_set_value(defaults, "view", "empty", "false");
    g_key_file_set_value(defaults, "view", "read", "false");

    std::string loc = std::string("location /var/mail/") + udata->pw_name;
    g_key_file_set_value(defaults, loc.c_str(), "skip", "false");
    loc = std::string("location ") + udata->pw_dir + "/Maildir";
    g_key_file_set_value(defaults, loc.c_str(), "skip", "false");
    loc = std::string("location ") + udata->pw_dir + "/Mail";
    g_key_file_set_value(defaults, loc.c_str(), "skip", "false");
    loc = std::string("location ") + udata->pw_dir + "/mail";
    g_key_file_set_value(defaults, loc.c_str(), "skip", "false");

#ifdef TRACE
    cerr << "Initial defaults: ";
    doc_defaults->write_to_stream_formatted(cerr);
#endif
}

std::vector<std::string> Config::secnames(const std::string& prefix)
{
    std::set<std::string> names;
    gchar** dsecs = g_key_file_get_groups(defaults, NULL);
    gchar** csecs = g_key_file_get_groups(cfg, NULL);

    // Merger names between defaults and cfg
    for (gchar** i = dsecs; *i; ++i)
        if (strncmp(*i, prefix.c_str(), prefix.size()) == 0)
            names.insert(*i + prefix.size());
    for (gchar** i = csecs; *i; ++i)
        if (strncmp(*i, prefix.c_str(), prefix.size()) == 0)
            names.insert(*i + prefix.size());

    g_strfreev(csecs);
    g_strfreev(dsecs);

    std::vector<std::string> res;
    std::copy(names.begin(), names.end(), std::back_inserter(res));

    return res;
}

void Config::clear()
{
    g_key_file_free(cfg);
    cfg = g_key_file_new();
}

static inline void inner_gerr_to_consistency(GError* err, const std::string& context)
{
    std::string msg = err->message;
    g_error_free(err);
    throw std::runtime_error(msg + "(" + context + ")");
}
// Optimise to avoid building context string if there is no error
#define gerr_to_consistency(err, context) do { \
    if (err != NULL) inner_gerr_to_consistency(err, context); \
} while (0)


void Config::load(const std::string& file)
{
    if (access(file.c_str(), F_OK) == -1)
        return;

    GError* err = NULL;
    char *text;
    gsize length;

    g_file_get_contents (file.c_str(), &text, &length, &err);
    gerr_to_consistency(err, "loading file " + file);

    try {
        if (length > 0)
        {
            g_key_file_load_from_file(cfg, file.c_str(), G_KEY_FILE_NONE, &err);
            gerr_to_consistency(err, "parsing .ini file " + file);
        }
    } catch (...) {
        g_free(text);
        throw;
    }

    g_free(text);
}

void Config::save()
{
    save(rcfile);
}

void Config::save(const std::string& file)
{
    GError* err = NULL;
    gsize length;
    gchar* data = g_key_file_to_data(cfg, &length, NULL);
    if (!data)
        throw std::runtime_error("cannot serialise configuration: g_key_file_to_data failed");

    if (length > 0)
    {
        try {
            g_file_set_contents(file.c_str(), data, length, &err);
            gerr_to_consistency(err, "saving file " + file);
        } catch (...) {
            g_free(data);
            throw;
        }
    }

    g_free(data);
}

void Config::dump()
{
    gsize length;
    gchar* data = g_key_file_to_data(cfg, &length, NULL);
    if (!data)
        throw std::runtime_error("cannot serialise configuration: g_key_file_to_data failed");

    fwrite(data, length, 1, stderr);

    g_free(data);
}

View Config::view()
{
    return View(*this, "view");
}

General Config::general()
{
    return General(*this, "general");
}

Folder Config::folder(std::shared_ptr<buffy::MailFolder> folder)
{
    return this->folder(folder->path());
}

Folder Config::folder(const std::string& folder)
{
    std::string secname = "folder " + folder;
    return Folder(*this, secname);
}

Section Config::application(const std::string& name)
{
    std::string secname = "app " + name;
    return Section(*this, secname);
}

std::vector<std::string> Config::locations()
{
    return secnames("location ");
}

Location Config::location(const std::string& location)
{
    std::string secname = "location " + location;
    // if (!g_key_file_has_group(cfg, secname.c_str()))
        // g_key_file_set_value(defaults, secname.c_str(), "skip", "false");
    return Location(*this, secname);
}

}
}
