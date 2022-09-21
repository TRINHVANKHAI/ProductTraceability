#ifndef USERINFO_H
#define USERINFO_H

#include <QDialog>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class UserInfo;
}

class UserInfo : public QDialog
{
    Q_OBJECT

public:
    explicit UserInfo(QWidget *parent = nullptr);
    ~UserInfo();
    QString getUserName();
    void saveUserName(QString usrName);

private slots:
    void on_regiserButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::UserInfo *ui;
};

#endif // USERINFO_H
