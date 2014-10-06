#include "buffy.h"
#include "ui_buffy.h"

Buffy::Buffy(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Buffy),
    folders_model(folders),
    sorterfilter(folders)
{
    ui->setupUi(this);
    sorterfilter.setSourceModel(&folders_model);
    ui->folders->setModel(&sorterfilter);
    ui->folders->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    connect(ui->action_quit, SIGNAL(triggered()), this, SLOT(do_quit()));
    connect(ui->action_refresh, SIGNAL(triggered()), &sorterfilter, SLOT(refresh()));

    folders_model.reread_folders();
}

Buffy::~Buffy()
{
    delete ui;
}

void Buffy::do_quit()
{
    close();
}
