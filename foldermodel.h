#ifndef PROVAMI_RAWQUERYMODEL_H
#define PROVAMI_RAWQUERYMODEL_H

#include <QAbstractTableModel>
#include <buffy/mailfolder.h>
#include "folders.h"

class FolderModel : public QAbstractTableModel
{
    Q_OBJECT

protected:
    Folders& folders;

public:
    enum ColumnType {
        CT_INVALID,
        CT_NAME,
        CT_NEW,
        CT_UNREAD,
        CT_TOTAL,
        CT_FLAGGED,
    };

    explicit FolderModel(Folders& folders, QObject *parent = 0);

    ColumnType resolveColumnType(int column) const;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    const Folder* valueAt(const QModelIndex& index) const;

signals:

public slots:
    void reread_folders();
};

#endif // RAWQUERYMODEL_H
