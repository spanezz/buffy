#include "buffy.h"
#include <QApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDebug>
#include "folders.h"
#include "dbus.h"

using namespace buffy;

int run_application(QApplication& a)
{
    Folders folders;
    config::Section prefs(folders.config.application("buffy"));
    prefs.addDefault("hidden", "false");

    Buffy w(a, folders);
    if (!prefs.getBool("hidden"))
        w.show();

    BuffyServer server(a, w);
    QDBusConnection::sessionBus().registerService("org.enricozini.buffy");
    QDBusConnection::sessionBus().registerObject("/buffy", &a);
    return a.exec();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!QDBusConnection::sessionBus().isConnected())
        return run_application(a);

    QDBusInterface iface("org.enricozini.buffy", "/buffy", "org.enricozini.buffy", QDBusConnection::sessionBus());
    if (!iface.isValid())
        return run_application(a);

    iface.call("set_visible", true);
    return 0;
}
