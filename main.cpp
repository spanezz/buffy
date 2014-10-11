#include "buffy.h"
#include <QApplication>
#include "folders.h"

using namespace buffy;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Folders folders;
    config::Section prefs(folders.config.application("buffy"));
    prefs.addDefault("hidden", "false");

    Buffy w(a, folders);
    if (!prefs.getBool("hidden"))
        w.show();

    return a.exec();
}
