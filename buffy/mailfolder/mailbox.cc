#include "buffy/mailfolder/mailbox.h"
#include "buffy/utils/sys.h"
#include "buffy/utils/system.h"
#include "from.h"

#include <sys/types.h>  // stat, opendir, readdir, utimes
#include <sys/stat.h>   // stat
#include <dirent.h>     // opendir, readdir
#include <cstring>
#include <sys/time.h>   // utimes

#include <zlib.h>

#include <system_error>
#include <cerrno>
#include <cstring>

using namespace buffy::utils;

namespace buffy {
namespace mailfolder {

Mailbox::Mailbox(const std::string& path) throw ()
    : _path(path), _stat_total(-1), _stat_unread(-1), _stat_new(-1), _stat_flagged(-1),
      _mbox_mtime(0), _mbox_size(0), _deleted(false)
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

Mailbox::Mailbox(const std::string& name, const std::string& path) throw ()
    : _name(name), _path(path), _stat_total(-1), _stat_unread(-1), _stat_new(-1), _stat_flagged(-1),
      _mbox_mtime(0), _mbox_size(0), _deleted(false) {}


static const int BUFFY_NEW      (1);
static const int BUFFY_READ     (1 << 1);
static const int BUFFY_FLAGGED  (1 << 2);

static int parse_mime_header (gzFile in, char* buf, int bufsize)
{
    bool status = false;
    int res = 0;

    while (gzgets(in, buf, bufsize)) 
        if (buf[0] == '\n')
            break;
        else if (strncmp(buf, "Status:", 7) == 0) 
        {
//          fprintf(stderr, "Has status: %s\n", buf);
            status = true;
            if (strchr(buf+7, 'R'))
                res |= BUFFY_READ;
            else if (!strchr (buf+7, 'O')) 
                res |= BUFFY_NEW;
        }
        else if (strncmp(buf, "X-Status:", 9) == 0) 
        {
//          fprintf(stderr, "Has x-status: %s\n", buf);
            if (strchr (buf+9, 'F')) 
                res |= BUFFY_FLAGGED;
        }

    if (!status)
        res |= BUFFY_NEW;

    return res;
}

bool Mailbox::changed()
{
    auto st = sys::stat(_path);
    if (!st.get())
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

    return st->st_mtime > _mbox_mtime || st->st_size != _mbox_size;
}

void Mailbox::updateStatistics()
{
    int res_total = 0;
    int res_read = 0;
    int res_new = 0;
    int res_flagged = 0;
    gzFile in = 0;

    /// Count messages in the 'new' directory

    // Perform consistency checks on the directory
    auto st = sys::stat(_path);
    if (!st.get())
    {
        _stat_total = 0;
        _stat_unread = 0;
        _stat_new = 0;
        _stat_flagged = 0;
        _deleted = true;
        return;
    }
    if (S_ISDIR(st->st_mode) != 0)
        throw std::runtime_error(_path + " is a directory");

    if (_deleted)
        _deleted = false;

    _mbox_mtime = st->st_mtime;
    _mbox_size = st->st_size;

//  if (!force && (s.st_size == mbox->file_size) && (s.st_mtime == mbox->file_mtime))
//      return 0;

    if (st->st_size == 0)
    {
        //mbox->file_size = 0;
        //mbox->file_mtime = 0;
        goto end2;
    }

    // TODO: lock the file
    // Count the messages
    in = gzopen(_path.c_str(), "rb");
    if (in == NULL)
        throw std::system_error(errno, std::system_category(), "cannot open file " + _path);

    static const int bufsize = 1024;
    char buf[bufsize];

    /* Check if a folder by checking for From as first thing in file */
    gzgets(in, buf, bufsize);
    if (!is_from(buf)) 
        goto end1;

    //mbox->file_mtime = s.st_mtime;
    //mbox->file_size = s.st_size;

    res_total = 1;
    if (int t = parse_mime_header(in, buf, bufsize)) 
    {
        if (t & BUFFY_NEW) res_new++;
        if (t & BUFFY_READ) res_read++;
        if (t & BUFFY_FLAGGED) res_flagged++;
    }

    while (gzgets(in, buf, bufsize))
        if (is_from(buf))
        {
            res_total++;
            if (int t = parse_mime_header (in, buf, bufsize))
            {
                if (t & BUFFY_NEW) res_new++;
                if (t & BUFFY_READ) res_read++;
                if (t & BUFFY_FLAGGED) res_flagged++;
            }
        }

end1:
    gzclose(in);


    // Restore the access time of the mailbox for other checking programs
    struct timeval t[2];
    t[0].tv_sec = st->st_atime;
    t[0].tv_usec = 0;
    t[1].tv_sec = st->st_mtime;
    t[1].tv_usec = 0;
    utimes(_path.c_str(), t);

    // Return the values
end2:
    _stat_total = res_total;
    _stat_unread = (res_total - res_read);// + res_new;
    _stat_new = res_new;
    _stat_flagged = res_flagged;
}


static bool isMailbox(const std::string& pathname)
{
    // Perform consistency checks on the file
    auto st = sys::stat(pathname);
    if (!st.get())
        return false;

    if (S_ISDIR(st->st_mode) != 0)
        return false;

    // Can be an empty file
    if (st->st_size == 0)
        return true;

    // Check if it starts with a From line
    gzFile in = gzopen(pathname.c_str(), "rb");
    if (in == NULL)
        throw std::system_error(errno, std::system_category(), "cannot open file " + pathname);

    static const int bufsize = 1024;
    char buf[bufsize];
    int res = gzread(in, buf, bufsize);
    if (res == 0)
    {
        // An empty gzip file is a valid mailbox
        if (gzeof(in))
        {
            gzclose(in);
            return true;
        } else {
            // Underlying read error: not a valid mbox
            gzclose(in);
            return false;
        }
    }
    if (res < 0)
    {
        // Some other error: not a valid mbox
        gzclose(in);
        return false;
    }

    // Read was correct, add a string terminator
    buf[res] = 0;

    // Check if a folder by checking for From as first thing in file
    gzclose(in);

    return is_from(buf);
}

MailFolder Mailbox::accessFolder(const std::string& path)
{
    try {
        if (isMailbox(path))
            return MailFolder(new Mailbox(path));
    } catch (std::exception& e) {
        // FIXME cerr << e.type() << ": " << e.fullInfo() << endl;
    }
    return MailFolder();
}


void Mailbox::enumerateFolders(const std::string& parent, MailFolderConsumer& cons)
{
    // Perform consistency checks on the parent directory
    struct stat st;
    if (stat(parent.c_str(), &st) != 0)
        return;
//      throw SystemException(errno, "getting informations on " + parent);

    if (isMailbox(parent))
    {
        MailFolder f(new Mailbox(parent));
        cons.consume(f);
    }

    if (S_ISDIR(st.st_mode) == 0)
        return;

    // Enumerate the Mailboxs in it
    sys::Path dir(parent);
    for (auto d = dir.begin(); d != dir.end(); ++d)
    {
        std::string name = d->d_name;
        if (name == ".") continue;
        if (name == "..") continue;
        const char* sep;
        if (!parent.empty() && parent[parent.size() - 1] != '/')
            sep = "/";
        else
            sep = "";
        std::string candidate = parent + sep + name;
        if (!sys::access(candidate, R_OK))
            continue;
        MailFolder f(accessFolder(candidate));
        if (f)
            cons.consume(f);
    }
}

}
}
