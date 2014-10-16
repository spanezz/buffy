#include "buffy.h"
#include "tray.h"
#include <QApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QCommandLineParser>
#include <QDebug>
#include "folders.h"
#include "dbus.h"
#include <iostream>

using namespace buffy;
using namespace std;

struct Main
{
    QApplication app;
    QCommandLineParser cmdline;
    QDBusInterface* remote_buffy = 0;
    QCommandLineOption set_active_inbox;
    QCommandLineOption unset_active_inbox;

    Main(int& argc, char** argv)
        : app(argc, argv),
          set_active_inbox("set-active-inbox", "Set the folder with the given name as an active inbox", "name"),
          unset_active_inbox("unset-active-inbox", "Set the folder with the given name to not be an active inbox", "name")

    {
        cmdline.setApplicationDescription("Email notification interface");
        cmdline.addHelpOption();
        cmdline.addVersionOption();
        cmdline.addOption(set_active_inbox);
        cmdline.addOption(unset_active_inbox);
        cmdline.process(app);

        if (QDBusConnection::sessionBus().isConnected())
        {
            remote_buffy = new QDBusInterface("org.enricozini.buffy", "/buffy", "org.enricozini.buffy", QDBusConnection::sessionBus());
            if (!remote_buffy->isValid())
            {
                delete remote_buffy;
                remote_buffy = 0;
            }
        }
    }
    ~Main()
    {
        if (remote_buffy) delete remote_buffy;
    }

    int run_gui()
    {
        Folders folders;
        folders.rescan();
        folders.refresh();

        config::Section prefs(folders.config.application("buffy"));
        prefs.addDefault("hidden", "false");

        Buffy w(app, folders);
        if (!prefs.getBool("hidden"))
            w.show();

        Tray tray(folders, w);
        tray.show();

        BuffyServer server(app, w);
        QDBusConnection::sessionBus().registerService("org.enricozini.buffy");
        QDBusConnection::sessionBus().registerObject("/buffy", &app);
        return app.exec();
    }

    void require_remote_buffy()
    {
        if (!remote_buffy)
        {
            cerr << "Buffy is not running" << endl;
            exit(1);
        }
    }
};

int main(int argc, char *argv[])
{
    Main main(argc, argv);

    if (main.cmdline.isSet(main.set_active_inbox))
    {
        main.require_remote_buffy();
        main.remote_buffy->call("set_active_inbox", main.cmdline.value(main.set_active_inbox), true);
        return 0;
    } else if (main.cmdline.isSet(main.unset_active_inbox)) {
        main.require_remote_buffy();
        main.remote_buffy->call("set_active_inbox", main.cmdline.value(main.unset_active_inbox), false);
        return 0;
    } else if (main.remote_buffy) {
        main.remote_buffy->call("set_visible", true);
        return 0;
    } else
        return main.run_gui();
}
