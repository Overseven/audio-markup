#include "graphcontrols.h"
#include "ui_graphcontrols.h"

GraphControls::GraphControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphControls)
{
    ui->setupUi(this);
    set_mode(Mode::Move);
}

GraphControls::~GraphControls()
{
    delete ui;
}

GraphControls::Mode GraphControls::get_current_mode()
{
    return mode;
}

void GraphControls::set_mode(Mode newMode)
{
    ui->pushButton_add->setChecked(false);
    ui->pushButton_move->setChecked(false);
    ui->pushButton_edit->setChecked(false);

    switch (newMode) {
    case Mode::Move:
        ui->pushButton_move->setChecked(true);
        break;
    case Mode::Add:
        ui->pushButton_add->setChecked(true);
        break;
    case Mode::Edit:
        ui->pushButton_edit->setChecked(true);
        break;
    }
    if (newMode != mode) {
        mode = newMode;
        emit mode_updated(newMode);
    }
}

void GraphControls::on_pushButton_add_clicked()
{
    set_mode(Mode::Add);
}


void GraphControls::on_pushButton_move_clicked()
{
    set_mode(Mode::Move);
}


void GraphControls::on_pushButton_edit_clicked()
{
    set_mode(Mode::Edit);
}

