#ifndef FOLDERS_H
#define FOLDERS_H

#include <buffy/config/config.h>
#include <buffy/mailfolder.h>
#include <functional>

class Folders
{
public:
    buffy::config::Config config;

    Folders();

    void find_folders(std::function<void(buffy::MailFolder)> func);
};

#endif // FOLDERS_H
