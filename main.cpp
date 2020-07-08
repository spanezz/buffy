#include "buffy.h"
#include "tray.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include "folders.h"
#include <iostream>

using namespace buffy;
using namespace std;

struct Main
{
    QApplication app;
    QCommandLineParser cmdline;
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
    }
    ~Main()
    {
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

        return app.exec();
    }
};

int main(int argc, char *argv[])
{
    Main main(argc, argv);
    return main.run_gui();
}
