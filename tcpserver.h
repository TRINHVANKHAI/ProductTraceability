#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include "tcpclient.h"

typedef enum _SUBMIT_STATUS_T {
    SUBMIT_STATUS_CPU_READY,
    SUBMIT_STATUS_CPU_DONE,
    SUBMIT_STATUS_LAN_READY,
    SUBMIT_STATUS_LAN_DONE,
    SUBMIT_STATUS_PENDING
} SUBMIT_STATUS_T;

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);
    ~TcpServer();
    int writeData(QTcpSocket *socket);
    void writeUserInfoData(QTcpSocket *socket);
    void postTestResultData(QByteArray data);
    void postOverallResultData(QByteArray data);
    void postSaveResultStatusData(QByteArray data);
    void showTestProgressData(QByteArray data);
    void popupQueryData(QByteArray data);
    void popupStatusData(QByteArray data);
    void popupStatusClose();
    int  getQueryResponseStatus();
    void setQueryResponseStatus(int res);
    void gotoCpuPageSwitch();
    void gotoLanPageSwitch();

signals:
    int submitStatusChange(SUBMIT_STATUS_T submitStatus);
    void postTestResults(QByteArray data);
    void postOverallTestResult(QByteArray data);
    void postSaveResultStatus(QByteArray data);
    void popupStatusMessage(QString data);
    void popupStatusMessageClose();
    void popupQueryMessage(QString data);
    void popupQueryMessageFinished();
    void gotoCpuPageSwitchClicked();
    void gotoLanPageSwitchClicked();
    void showTestProgress(QString msg);

public slots:
    void on_popupQueryFinished(int res);
    int on_CpuDataSubmitted(QString serialNumber);
    int on_LanDataSubmitted(QString serialNumber, QString macAddress);
    void on_UserNameDataSubmitted(QString usrName);

private:
    char submitSerialNumber[30];
    char submitMacAddress[17];
    QString submitUserName;
    SUBMIT_STATUS_T submitStatus;
    TcpClient *client;
    int dialogResponse = 0xff;
protected:
    void incomingConnection(qintptr socketDescriptor);

};

#endif // TCPSERVER_H
