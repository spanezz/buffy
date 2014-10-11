#ifndef BUFFY_DBUS_H
#define BUFFY_DBUS_H

#include <QtDBus/QDBusAbstractAdaptor>
#include <QApplication>
#include "buffy.h"

class BuffyServer : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.enricozini.buffy")

protected:
    Buffy& buffy;

public:
    explicit BuffyServer(QApplication& app, Buffy& buffy);

signals:

public slots:
    void set_active_inbox(QString folder_name, bool value);
    void set_visible(bool value);
};

#endif // DBUS_H
