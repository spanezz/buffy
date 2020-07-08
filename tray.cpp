#include "tray.h"
#include <QDebug>

using namespace buffy;

Tray::Tray(Folders& folders, Buffy &buffy, QObject *parent) :
    QSystemTrayIcon(parent), folders(folders), buffy(buffy)
{
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(tray_activated(QSystemTrayIcon::ActivationReason)));
    connect(&folders, SIGNAL(visibility_updated()), this, SLOT(visibility_updated()));

    tray_menu.addAction(buffy.action_quit());
    setContextMenu(&tray_menu);

    visibility_updated();
}

void Tray::tray_activated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        //QSystemTrayIcon::Unknown	0	Unknown reason
        case QSystemTrayIcon::Context:
        {
            // The context menu for the system tray entry was requested
            tray_menu.clear();
            bool has_inboxes = false;
            folders.each_active_inbox([&](Folder& f){
                tray_menu.addAction(new ActivateInboxAction(f, &tray_menu));
                has_inboxes = true;
            });
            if (has_inboxes)
                tray_menu.addSeparator();
            tray_menu.addAction(buffy.action_quit());
            break;
        }
        case QSystemTrayIcon::MiddleClick:
        case QSystemTrayIcon::DoubleClick:
        case QSystemTrayIcon::Trigger:
        {
            config::Section prefs(folders.config.application("buffy"));

            // The system tray entry was clicked
            if (buffy.isVisible())
            {
                buffy.do_hide();
            } else {
                buffy.do_show();
            }
            break;
        }
        default:
            break;
    }
}

void Tray::visibility_updated()
{
    QIcon icon;
    if (folders.has_active_new())
        icon = QIcon(":/icons/mail-new");
    else
        icon = QIcon(":/icons/mail-closed");
    setIcon(icon);
}
