#include "buffy.h"
#include "ui_buffy.h"

Buffy::Buffy(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Buffy),
    folders_model(folders)
{
    ui->setupUi(this);
    ui->folders->setModel(&folders_model);
    ui->folders->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    folders_model.reread_folders();
}

Buffy::~Buffy()
{
    delete ui;
}

void Buffy::on_action_Quit_triggered()
{
    close();
}
