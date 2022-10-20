#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QLabel>
#include <QKeyEvent>
#include "tcpserver.h"
#include "tcpclient.h"
#include "statusitemwidget.h"
#include "dialog.h"
#include "popupstatus.h"
#include "userinfo.h"
#include "fileexport.h"

typedef enum _PT_PAGE_STAGE_T {
    PT_PAGE_STAGE_HOME,
    PT_PAGE_STAGE_CPU,
    PT_PAGE_STAGE_LAN
} PT_PAGE_STAGE_T;

typedef enum _PT_INPUT_FIELD_T {
    PT_INPUT_FIELD_NONE,
    PT_INPUT_FIELD_SERIAL_NUMBER,
    PT_INPUT_FIELD_MAC_ADDRESS
} PT_INPUT_FIELD_T;


namespace Ui {
class MainMenu;
}

class MainMenu : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = 0);
    ~MainMenu();
    void clearTestResults();
    void showTestResult(QString statusNo, QString statusName, QString sts);

signals:
    int submitCpuData(QString serialNumber);
    int submitLanData(QString serialNumber, QString macAddress);
    void submitUserNameData(QString usrName);

public slots:
    int on_submitStatusChanged(SUBMIT_STATUS_T submitStatus);
    void on_postTestResults(QByteArray data);
    void on_postOverallTestResult(QByteArray data);
    void on_postSaveResultStatus(QByteArray data);
    void on_popupQueryMessage(QString msg);
    void on_popupQueryMessageFinished();
    void on_popupStatusMessage(QString msg);
    void on_popupStatusMessageClose();
    void on_showTestProgress(QString msg);
private slots:
    void on_cpuButton_clicked();

    void on_lanButton_clicked();

    void on_cpuGoHome_clicked();

    void on_lanGoHome_clicked();
    void on_changeUserNameButton_clicked();
    void on_UserInfo_changed();

    void on_actionOpen_triggered();

    void on_actionImport_triggered();

    void on_actionExport_triggered();

    void on_actionSave_triggered();

    void on_actionExit_triggered();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::MainMenu *ui;
    TcpServer *server;
    QWidget *currentPage;
    Dialog *popupQuery=nullptr;
    PopupStatus *popupStatus=nullptr;
    UserInfo *userInfoForm;
    QListWidgetItem *cpuWidgetItem;
    QListWidgetItem *lanWidgetItem;
    PT_INPUT_FIELD_T inputField;
    QString inputValue;
    QLabel *inputFieldShow;
    QString serialNumber;
    QString macAddress;
    unsigned int overallTestStatus;
    unsigned int overallTestCount;
    void eventKeyPressed(QKeyEvent* event);
};



#endif // MAINMENU_H
