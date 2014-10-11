#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include <QTableView>
#include <QAction>
#include <buffy/config/config.h>

class FolderAction : public QAction
{
protected:
    buffy::config::Folder foldercfg;

public:
    FolderAction(buffy::config::Folder foldercfg, QObject *parent)
        : QAction(parent), foldercfg(foldercfg)
    {
        setCheckable(true);
    }
};

struct ViewAlwaysAction : public FolderAction
{
    Q_OBJECT

public:
    ViewAlwaysAction(buffy::config::Folder& foldercfg, QObject *parent)
        : FolderAction(foldercfg, parent)
    {
        setChecked(foldercfg.forceview());
        setText("View always");
        connect(this, SIGNAL(triggered(bool)), this, SLOT(set_value(bool)));
    }

public slots:
    void set_value(bool val)
    {
        foldercfg.setForceView(val);
    }
};

struct HideAlwaysAction : public FolderAction
{
    Q_OBJECT

public:
    HideAlwaysAction(buffy::config::Folder& foldercfg, QObject *parent)
        : FolderAction(foldercfg, parent)
    {
        setChecked(foldercfg.forcehide());
        setText("Hide always");
        connect(this, SIGNAL(triggered(bool)), this, SLOT(set_value(bool)));
    }

public slots:
    void set_value(bool val)
    {
        foldercfg.setForceHide(val);
    }
};

struct ActiveInboxAction : public FolderAction
{
    Q_OBJECT

public:
    ActiveInboxAction(buffy::config::Folder& foldercfg, QObject *parent)
        : FolderAction(foldercfg, parent)
    {
        foldercfg.addDefault("activeinbox", "false");
        setChecked(foldercfg.getBool("activeinbox"));
        setText("Active inbox");
        connect(this, SIGNAL(triggered(bool)), this, SLOT(set_value(bool)));
    }

public slots:
    void set_value(bool val)
    {
        foldercfg.setBool("activeinbox", val);
    }
};

class FolderView : public QTableView
{
    Q_OBJECT

protected:
    void contextMenuEvent(QContextMenuEvent *event);

public:
    explicit FolderView(QWidget *parent = 0);

signals:

public slots:

};

#endif // FOLDERVIEW_H
