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
            if (isVisible())
            {
                buffy.do_hide();
            } else {
                buffy.do_show();
            }
            break;
        }
    }
}

void Tray::visibility_updated()
{
    //icon_read(":/icons/mail-closed"),
    //icon_unread(":/icons/mail-new"),
    //icon_read(QIcon::fromTheme("mail-read")),
    //icon_unread(QIcon::fromTheme("mail-unread")),
    //        QIcon foo(":/icons/mail-new");
    //        qDebug() << "ICONS " << icon_read << " " << icon_unread << " " << foo;
    //        qDebug() << "ICON READ " << icon_read.isNull() << " " << icon_read.name();

    QIcon icon;
    if (folders.has_active_new())
    {
        icon = QIcon(":/icons/mail-new");
    }
    else
    {
        icon = QIcon(":/icons/mail-closed");
    }
    qDebug() << "ISNULL" << icon.isNull();
    setIcon(icon);
}
