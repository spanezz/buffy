#include "dbus.h"
#include <QDebug>

BuffyServer::BuffyServer(QApplication& app, Buffy& buffy) :
    QDBusAbstractAdaptor(&app), buffy(buffy)
{
}

void BuffyServer::set_active_inbox(QString folder_name, bool value)
{
    Folder* f = buffy.folders.by_name(folder_name.toStdString());
    f->set_active_inbox(value);
}

void BuffyServer::set_visible(bool value)
{
    if (value)
        buffy.do_show();
    else
        buffy.do_hide();
}
