#include "popupstatus.h"
#include "ui_popupstatus.h"

PopupStatus::PopupStatus(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PopupStatus)
{
    ui->setupUi(this);
}

PopupStatus::~PopupStatus()
{
    delete ui;
}

void PopupStatus::setDialogMessage(QString msg) {
    ui->messageLabel->setText(msg);
}
