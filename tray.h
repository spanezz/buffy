#ifndef TRAY_H
#define TRAY_H

#include "folders.h"
#include "buffy.h"
#include <QSystemTrayIcon>
#include <QMenu>

class Tray : public QSystemTrayIcon
{
    Q_OBJECT

protected:
    Folders& folders;
    Buffy& buffy;
    QMenu tray_menu;

public:
    explicit Tray(Folders& folders, Buffy& buffy, QObject *parent = 0);

public slots:

private slots:
    void tray_activated(QSystemTrayIcon::ActivationReason reason);
    void visibility_updated();
};

#endif // TRAY_H
