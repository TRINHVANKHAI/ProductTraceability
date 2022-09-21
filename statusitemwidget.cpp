#include "statusitemwidget.h"
#include "ui_statusitemwidget.h"
#include <QDebug>

statusItemWidget::statusItemWidget(QString statusNo, QString statusDesc, QString status, QListWidgetItem *item, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::statusItemWidget)
{
    ui->setupUi(this);
    ui->labelNum->setText(statusNo);
    ui->labelStatus->setText(statusDesc);
    ui->labelIcon->setText(status);
    if (status.startsWith("PASSED")) {
        ui->labelNum->setStyleSheet("QLabel { background-color : rgb(230, 235, 245); color : green; }");
        ui->labelStatus->setStyleSheet("QLabel { background-color : rgb(230, 235, 245); color : green; }");
        ui->labelIcon->setStyleSheet("QLabel { background-color : rgb(230, 235, 245); color : green; }");
    } else {
        ui->labelNum->setStyleSheet("QLabel { background-color : rgb(247, 255, 189); color : red; }");
        ui->labelStatus->setStyleSheet("QLabel { background-color : rgb(247, 255, 189); color : red; }");
        ui->labelIcon->setStyleSheet("QLabel { background-color : rgb(247, 255, 189); color : red; }");
    }
}

statusItemWidget::~statusItemWidget()
{
    delete ui;
}
