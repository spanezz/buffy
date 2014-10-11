#include "preferences.h"
#include "ui_preferences.h"
#include <memory>
#include <set>
#include <algorithm>
#include <QMenu>
#include <QFileDialog>
#include <QDebug>

using namespace buffy;

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    mail_programs(0, 2),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    ui->folder_locations->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->folder_locations, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(folder_list_context_menu(QPoint)));

    QStringList headers;
    headers << "Name" << "Command";
    mail_programs.setHorizontalHeaderLabels(headers);
    ui->email_programs->setModel(&mail_programs);
    ui->email_program->setModel(&mail_programs);
    ui->email_program->setModelColumn(0);

    ui->email_programs->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->email_programs, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(email_programs_context_menu(QPoint)));
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::from_config(config::Config &config)
{
    // Update interval
    ui->update_interval->setText(QString::number(config.general().interval()));

    // Folder locations
    ui->folder_locations->clear();
    auto locations = config.locations();
    for (auto l: locations)
    {
        std::unique_ptr<QListWidgetItem> item(new QListWidgetItem(ui->folder_locations));
        item->setText(QString::fromStdString(l));
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->folder_locations->addItem(item.release());
    }

    // Email programs
    mail_programs.clear();
    std::vector<std::string> progs(config.mailPrograms());
    int selected = 0;
    for (size_t i = 0; i < progs.size(); ++i)
    {
        const std::string& name = progs[i];
        config::MailProgram mp = config.mailProgram(name);
        if (mp.selected()) selected = i;
        QList<QStandardItem*> row;
        row.append(new QStandardItem(QString::fromStdString(mp.name())));
        row.append(new QStandardItem(QString::fromStdString(mp.command("gui"))));
        mail_programs.appendRow(row);
    }
    ui->email_program->setCurrentIndex(selected);
}

void Preferences::to_config(config::Config &config)
{
    // Update interval
    config.general().setInterval(ui->update_interval->text().toInt());

    // Folder locations
    // Buffy currently support adding locations but not removing locations
    std::vector<std::string> locations = config.locations();
    std::set<std::string> location_set;
    std::copy(locations.begin(), locations.end(), std::inserter(location_set, location_set.end()));
    for(int i = 0; i < ui->folder_locations->count(); ++i)
    {
        QListWidgetItem* item = ui->folder_locations->item(i);
        std::string loc = item->text().toStdString();
        if (location_set.find(loc) == location_set.end())
            // Add new location
            config.location(loc);
    }

    // Email programs
    for(int row = 0; row < mail_programs.rowCount(); ++row)
    {
        QStandardItem* item = mail_programs.item(row, 0);
        std::string name = item->text().toStdString();
        item = mail_programs.item(row, 1);
        std::string command = item->text().toStdString();
        config.mailProgram(name).setCommand("gui", command);
    }
    config.selectMailProgram(ui->email_program->currentText().toStdString());
}

void Preferences::folder_list_context_menu(const QPoint &pos)
{
    QPoint globalPos = ui->folder_locations->mapToGlobal(pos);
    QListWidgetItem* item = ui->folder_locations->itemAt(pos);
    QMenu menu(this);
    QAction* add_action = menu.addAction("New location");
    QAction* del_action = 0;
    if (item)
        del_action = menu.addAction("Delete location");
    QAction* res = menu.exec(globalPos);
    if (!res) return;
    if (res == add_action)
    {
        QString file_name = QFileDialog::getOpenFileName(this, "New location");
        std::unique_ptr<QListWidgetItem> item(new QListWidgetItem(ui->folder_locations));
        item->setText(file_name);
        ui->folder_locations->addItem(item.release());
    }
    else if (res == del_action)
    {
        delete ui->folder_locations->takeItem(ui->folder_locations->row(item));
    }
}

void Preferences::email_programs_context_menu(const QPoint &pos)
{
    QPoint globalPos = ui->email_programs->mapToGlobal(pos);
    QModelIndex index = ui->email_programs->indexAt(pos);
    QStandardItem* item = index.isValid() ? mail_programs.itemFromIndex(index) : 0;
    QMenu menu(this);
    QAction* add_action = menu.addAction("New program");
    QAction* del_action = 0;
    if (item)
        del_action = menu.addAction("Delete program");
    QAction* res = menu.exec(globalPos);
    if (!res) return;
    if (res == add_action)
    {
        QList<QStandardItem*> row;
        row.append(new QStandardItem("new_program"));
        row.append(new QStandardItem("/usr/bin/sample-mail-editor --folder %p"));
        mail_programs.appendRow(row);
        ui->email_programs->edit(mail_programs.indexFromItem(row[0]));
    }
    else if (res == del_action)
    {
        auto row = mail_programs.takeRow(index.row());
        for (auto i : row)
            delete i;
    }
}
