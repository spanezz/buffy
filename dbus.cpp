#include "dbus.h"
#include <QDebug>

BuffyServer::BuffyServer(QApplication& app, Buffy& buffy) :
    QDBusAbstractAdaptor(&app), buffy(buffy)
{
}

void BuffyServer::set_active_inbox(QString folder_name, bool value)
{
    buffy.set_active_inbox(folder_name, value);
}

void BuffyServer::set_visible(bool value)
{
    if (value)
        buffy.do_show();
    else
        buffy.do_hide();
}
