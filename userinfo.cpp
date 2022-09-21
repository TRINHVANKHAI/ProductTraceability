#include "userinfo.h"
#include "ui_userinfo.h"

#include <QDebug>

UserInfo::UserInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserInfo)
{
    ui->setupUi(this);
}

UserInfo::~UserInfo()
{
    delete ui;
}

void UserInfo::on_regiserButton_clicked()
{
    QString userName = ui->userInput->text();
    this->saveUserName(userName);
    this->accept();
}


void UserInfo::on_cancelButton_clicked()
{
    this->reject();
}

QString UserInfo::getUserName() {
    QFile usrInfoFile("usrinfo.json");
    QString userName;
    userName.clear();
    if(!usrInfoFile.open((QIODevice::ReadOnly))) {
        qWarning("Couldn't open file to read data");
        return userName;
    }

    QByteArray usrInfoData = usrInfoFile.readAll();
    usrInfoFile.close();
    QJsonObject usrInfoJson(QJsonDocument::fromJson(usrInfoData).object());
    userName = usrInfoJson["username"].toString();
    return userName;
}


void UserInfo::saveUserName(QString usrName) {
    QJsonObject usrNameJson;
    usrNameJson["username"] = usrName;
    QJsonDocument usrNameJDoc(usrNameJson);

    QFile usrInfoFile("usrinfo.json");
    if(!usrInfoFile.open((QIODevice::WriteOnly))) {
        qWarning("Couldn't open file to save data");
        return;
    }

    usrInfoFile.write(usrNameJDoc.toJson());
    usrInfoFile.close();
}
