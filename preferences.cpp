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
