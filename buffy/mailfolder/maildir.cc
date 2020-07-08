#include "buffy/mailfolder/maildir.h"
#include "buffy/utils/sys.h"
#include "buffy/utils/string.h"

#include <sys/types.h>  // stat, opendir, readdir, utimes
#include <sys/stat.h>   // stat
#include <dirent.h>     // opendir, readdir
#include <cstring>
#include <sys/time.h>   // utimes

#include <set>
#include <cerrno>
#include <cstring>

using namespace buffy::utils;

namespace buffy {
namespace mailfolder {

static std::string concat(const std::string& str1, char sep, const std::string& str2)
{
    // Avoid adding a separator if str1 or str are empty, if str1 ends with the
    // separator or if str2 starts with the separator
    if (!str1.empty() && str1[str1.size() - 1] != sep && ! str2.empty() && str2[0] != sep)
        return str1 + sep + str2;
    else
        return str1 + str2;
}

Maildir::Maildir(const std::string& path) throw ()
    : _path(path), _stat_total(-1), _stat_unread(-1), _stat_new(-1), _stat_flagged(-1),
      _deleted(false), _new_mtime(0), _cur_mtime(0)
{
    _name = _path;

    /// Normalize the folder name

    // Remove trailing '/'
    while (_name[_name.size() - 1] == '/')
        _name.resize(_name.size() - 1);

    // Remove leading path
    size_t lastslash = _name.find_last_of('/');
    if (lastslash != std::string::npos)
        _name = _name.substr(lastslash + 1);

    // Remove leading dot
    if (_name[0] == '.')
        _name = _name.substr(1);
}

Maildir::Maildir(const std::string& name, const std::string& path) throw ()
    : _name(name), _path(path), _stat_total(-1), _stat_unread(-1), _stat_new(-1), _stat_flagged(-1),
      _deleted(false), _new_mtime(0), _cur_mtime(0) {}


bool Maildir::changed()
{
    // Compute 'new' and 'cur' directory names
    std::string path_new = _path + "/new";
    std::string path_cur = _path + "/cur";

    auto st_new = sys::stat(path_new);
    if (!st_new.get())
        if (! _deleted)
        {
            _deleted = true;
            return true;
        }

    auto st_cur = sys::stat(path_cur);
    if (!st_cur.get())
        if (! _deleted)
        {
            _deleted = true;
            return true;
        }

    if (_deleted)
    {
        _deleted = false;
        return true;
    }

    return st_new->st_mtime > _new_mtime || st_cur->st_mtime > _cur_mtime;
}

void Maildir::updateStatistics()
{
    int res_total = 0;
    int res_unread = 0;
    int res_new = 0;
    int res_flagged = 0;

    // Compute 'new' and 'cur' directory names
    std::string path_new = _path + "/new";
    std::string path_cur = _path + "/cur";

    // Perform consistency checks on the 'new' directory
    auto st_new = sys::stat(path_new);
    if (!st_new.get())
    {
        _stat_total = 0;
        _stat_unread = 0;
        _stat_new = 0;
        _stat_flagged = 0;
        _deleted = true;
        return;
    }
    if (S_ISDIR(st_new->st_mode) == 0)
        throw std::runtime_error(path_new + " is not a directory");

    // Perform consistency checks on the 'cur' directory
    auto st_cur = sys::stat(path_cur);
    if (!st_cur.get())
    {
        _stat_total = 0;
        _stat_unread = 0;
        _stat_new = 0;
        _stat_flagged = 0;
        _deleted = true;
        return;
    }
    if (S_ISDIR(st_cur->st_mode) == 0)
        throw std::runtime_error(path_cur + " is not a directory");

    if (_deleted)
        _deleted = false;

    _new_mtime = st_new->st_mtime;
    _cur_mtime = st_cur->st_mtime;

    /// Count messages in the 'new' directory

    // Count the files in the 'new' directory
    {
        sys::Path dir(path_new);
        for (auto d = dir.begin(); d != dir.end(); ++d)
        {
            if (d->d_name[0] == '.')
                continue;
            res_total++;
            res_new++;
        }
    }

    // Restore the access time of the mailbox for other checking programs
    struct timeval t[2];
    t[0].tv_sec = st_new->st_atime;
    t[0].tv_usec = 0;
    t[1].tv_sec = st_new->st_mtime;
    t[1].tv_usec = 0;
    utimes(path_new.c_str(), t);


    /// Count messages in the 'cur' directory

    // Count the files in the 'cur' directory
    {
        sys::Path dir(path_cur);
        for (auto d = dir.begin(); d != dir.end(); ++d)
        {
            if (d->d_name[0] == '.') continue;
            std::string name = d->d_name;

            res_total++;

            // Look for an `info' block in the name
            size_t p_info = name.rfind(':');
            if (p_info == std::string::npos) continue;

            // Ensure that the info block is in the right format
            if (name.compare(p_info, 3, ":2,") != 0) continue;

            // Look for the 'S' flag (it should not be there)
            p_info += 3;
            if (name.find('S', p_info) == std::string::npos)
                res_unread++;
            if (name.find('F', p_info) != std::string::npos)
                res_flagged++;
        }
    }

    // Restore the access time of the mailbox for other checking programs
    t[0].tv_sec = st_cur->st_atime;
    t[0].tv_usec = 0;
    t[1].tv_sec = st_cur->st_mtime;
    t[1].tv_usec = 0;
    utimes(path_cur.c_str(), t);


    // Return the values
    _stat_total = res_total;
    _stat_unread = res_unread + res_new;
    _stat_new = res_new;
    _stat_flagged = res_flagged;
}


static bool isMaildir(const std::string& pathname)
{
    try {
        // It must be a directory
        auto st = sys::stat(pathname);
        if (!st.get())
            return false;
        if (S_ISDIR(st->st_mode) == 0)
            return false;

        // It must contain cur, new and tmp subdirectories
        const char* subdirs[3] = { "cur", "new", "tmp" };
        for (int i = 0; i < 3; i++)
            if (!sys::isdir(str::joinpath(pathname, subdirs[i])))
                return false;

        // It appears to be a maildir directory
        return true;
    } catch (std::system_error& e) {
        // If we can't even stat() it, then it's not a maildir
        return false;
    }
}

std::shared_ptr<MailFolder> Maildir::accessFolder(const std::string& path)
{
    try {
        if (isMaildir(path))
            return std::make_shared<Maildir>(path);
    } catch (std::system_error& e) {
        // FIXME: cerr << e.type() << ": " << e.fullInfo() << endl;
    }
    return std::shared_ptr<Maildir>();
}

static void enumerateSubfolders(
        const std::string& parent,
        const std::string& name,
        std::function<void(std::shared_ptr<MailFolder>)> cons,
        std::set<ino_t> seen = std::set<ino_t>())
{
    try {
        std::unique_ptr<struct stat> st;
        try {
            st = sys::stat(parent);
        } catch (std::system_error& e) {
            // If we can't even stat() it, then we don't try to enumerate its subfolders
            // FIXME: cerr << e.type() << ": " << e.fullInfo() << endl;
            return;
        }
        if (!st.get()) return;

        // It must be a directory
        if (S_ISDIR(st->st_mode) == 0)
            return;

        // Check that we aren't looping
        if (seen.find(st->st_ino) != seen.end())
            return;

        if (isMaildir(parent))
            cons(std::make_shared<Maildir>(name, parent));

        // Recursively enumerate the Maildirs in the directory
        sys::Path dir(parent);
        for (auto d = dir.begin(); d != dir.end(); ++d)
        {
            std::string dname = d->d_name;
            if (dname == ".") continue;
            if (dname == "..") continue;
            if (dname == "tmp") continue;
            if (dname == "cur") continue;
            if (dname == "new") continue;

            std::set<ino_t> sub = seen;
            sub.insert(st->st_ino);
            enumerateSubfolders(
                    concat(parent, '/', dname),
                    concat(name, '.', dname),
                    cons, sub);
        }
    } catch (std::exception& e) {
        // FIXME: cerr << e.type() << ": " << e.fullInfo() << endl;
    }
}

void Maildir::enumerateFolders(const std::string& parent, std::function<void(std::shared_ptr<MailFolder>)> cons)
{
    // Remove trailing slash from the parent directory
    // The root name is empty if parent is not a maildir
    //   else, it is the last component of parent's path
    std::string root;
    std::string rootName;

    size_t pos = parent.rfind('/');
    if (pos == std::string::npos)
        root = rootName = parent;
    else if (pos == parent.size() - 1)
    {
        pos = parent.rfind('/', pos - 1);
        root = parent.substr(0, parent.size() - 1);
        rootName = parent.substr(pos+1, parent.size() - pos - 2);
    }
    else
    {
        root = parent;
        rootName = parent.substr(pos + 1);
    }

    if (!isMaildir(parent))
        rootName = std::string();

    enumerateSubfolders(root, rootName, cons);
}

}
}
