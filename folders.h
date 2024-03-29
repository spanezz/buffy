#ifndef FOLDERS_H
#define FOLDERS_H

#include "backend/config.h"
#include "backend/mailfolder.h"
#include <functional>
#include <QAbstractTableModel>
#include <QAction>
#include <QProcess>

class Folders;

class Folder : public QObject
{
    Q_OBJECT

public:
    Folders& folders;
    std::shared_ptr<buffy::MailFolder> folder;

    Folder(Folders& folders,
           std::shared_ptr<buffy::MailFolder> folder,
           QObject* parent=0);

    buffy::config::Folder cfg();
    buffy::config::Folder cfg() const;
    bool is_visible() const;
    std::string name() const;

public slots:
    void set_active_inbox(bool value);
    void run_email_program();
    void on_mua_finished(int exitCode, QProcess::ExitStatus exitStatus);

};

class Folders: public QAbstractTableModel
{
    Q_OBJECT

protected:
    std::vector<Folder*> all;

public:
    buffy::config::Config config;

    explicit Folders(QObject* parent=0);
    ~Folders();

    bool has_active_new() const;

    void set_visibility(bool view_all, bool view_all_nonempty, bool view_all_flagged, bool view_only_active_inboxes);

    Folder* valueAt(const QModelIndex& index);
    void each_active_inbox(std::function<void(Folder&)> func);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList& indexes) const override;

signals:
    void visibility_updated();

public slots:
    /// Look for folders on disk
    void rescan();

    /// Update folder statistics
    void refresh();
};


class FolderAction : public QAction
{
protected:
    Folder& folder;

public:
    FolderAction(Folder& folder, QObject *parent)
        : QAction(parent), folder(folder)
    {
        setCheckable(true);
    }
};

struct ViewAlwaysAction : public FolderAction
{
    Q_OBJECT

public:
    ViewAlwaysAction(Folder& folder, QObject *parent)
        : FolderAction(folder, parent)
    {
        setChecked(folder.cfg().forceview());
        setText("View always");
        connect(this, SIGNAL(triggered(bool)), this, SLOT(set_value(bool)));
    }

public slots:
    void set_value(bool val)
    {
        folder.cfg().setForceView(val);
    }
};

struct HideAlwaysAction : public FolderAction
{
    Q_OBJECT

public:
    HideAlwaysAction(Folder& folder, QObject *parent)
        : FolderAction(folder, parent)
    {
        setChecked(folder.cfg().forcehide());
        setText("Hide always");
        connect(this, SIGNAL(triggered(bool)), this, SLOT(set_value(bool)));
    }

public slots:
    void set_value(bool val)
    {
        folder.cfg().setForceHide(val);
    }
};

struct Hide24hAction : public FolderAction
{
    Q_OBJECT

public:
    Hide24hAction(Folder& folder, QObject *parent)
        : FolderAction(folder, parent)
    {
        setChecked(folder.cfg().forcehide());
        setText("Hide for 24h");
        connect(this, SIGNAL(triggered(bool)), this, SLOT(set_value(bool)));
    }

public slots:
    void set_value(bool val)
    {
        if (val)
            folder.cfg().setHideUntil(time(nullptr) + 86400);
        else
            folder.cfg().setHideUntil(0);
    }
};

struct ActiveInboxAction : public FolderAction
{
    Q_OBJECT

public:
    ActiveInboxAction(Folder& folder, QObject *parent)
        : FolderAction(folder, parent)
    {
        setChecked(folder.cfg().getBool("activeinbox"));
        setText("Active inbox");
        connect(this, SIGNAL(triggered(bool)), &folder, SLOT(set_active_inbox(bool)));
    }
};

class ActivateInboxAction: public FolderAction
{
    Q_OBJECT

public:
    ActivateInboxAction(Folder& folder, QObject* parent)
        : FolderAction(folder, parent)
    {
        connect(this, SIGNAL(triggered()), &folder, SLOT(run_email_program()));
        QString name(QString::fromStdString(folder.folder->name()));
        name += QString(" (%1 new)").arg(folder.folder->getMsgNew());
        setText(name);
    }
};

#endif // FOLDERS_H
