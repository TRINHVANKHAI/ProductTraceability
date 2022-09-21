#include "tcpserver.h"
#include "tcpclient.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{

    submitStatus = SUBMIT_STATUS_PENDING;

    if(!this->listen(QHostAddress::AnyIPv4, 9999)) {
        qDebug() << "SERVER:: Server could not start";
    } else {
        qDebug() << "SERVER:: Server started successfully";
    }
}

TcpServer::~TcpServer() {
    qDebug() << "SERVER:: Server terminated gracefully";
    submitStatus = SUBMIT_STATUS_PENDING;
}


void TcpServer::incomingConnection(qintptr socketDescriptor) {
    qDebug() << "SERVER:: New client connected " << socketDescriptor;
    client = new TcpClient(socketDescriptor, this);
    connect(client, SIGNAL(finished()), client, SLOT(deleteLater()));

    client->start();
}

int TcpServer::on_CpuDataSubmitted(QString serialNumber) {
    QByteArray dataBytes;
    char *data;

    if(serialNumber.size()==30) {
        dataBytes = serialNumber.toLocal8Bit();
        data = dataBytes.data();
        memcpy(submitSerialNumber, data, serialNumber.size());
        submitStatus = SUBMIT_STATUS_CPU_READY;
        emit submitStatusChange(SUBMIT_STATUS_CPU_READY);
        return 0;
    } else {
        submitStatus = SUBMIT_STATUS_PENDING;
        return -1;
    }
}

int TcpServer::on_LanDataSubmitted(QString serialNumber, QString macAddress) {

    QByteArray dataBytes;
    char *data;
    int i;
    if((serialNumber.size()==30) && (macAddress.size()==5)) {
        dataBytes = serialNumber.toLocal8Bit();
        data = dataBytes.data();
        memcpy(submitSerialNumber, data, serialNumber.size());

        macAddress = "8439BE0" + macAddress;
        for(i=2;i<16;i+=3) {
            macAddress.insert(i, ':');
        }

        dataBytes = macAddress.toLocal8Bit();
        data = dataBytes.data();
        memcpy(submitMacAddress, data, macAddress.size());

        submitStatus = SUBMIT_STATUS_LAN_READY;
        emit submitStatusChange(SUBMIT_STATUS_LAN_READY);
        return 0;
    } else {
        submitStatus = SUBMIT_STATUS_PENDING;
        return -1;
    }
    return 0;
}

void TcpServer::on_UserNameDataSubmitted(QString usrName) {
    submitUserName = usrName;
}

void TcpServer::writeUserInfoData(QTcpSocket *socket) {
    QByteArray dataBytes = submitUserName.toUtf8();
    char *usrNameData = dataBytes.data();
    socket->write(usrNameData, dataBytes.size());
    socket->flush();
    socket->write(",", 1);
    socket->flush();
}

int TcpServer::writeData(QTcpSocket *socket) {
    switch(submitStatus) {
    case SUBMIT_STATUS_CPU_READY:
        socket->write(submitSerialNumber, sizeof(submitSerialNumber));
        socket->flush();
        socket->waitForBytesWritten(30000);
        submitStatus = SUBMIT_STATUS_CPU_DONE;
        emit submitStatusChange(SUBMIT_STATUS_CPU_DONE);
        break;
    case SUBMIT_STATUS_LAN_READY:
        socket->write(submitSerialNumber, sizeof(submitSerialNumber));
        socket->flush();
        socket->waitForBytesWritten(30000);
        socket->write(",", 1);
        socket->flush();
        socket->waitForBytesWritten(30000);
        socket->write(submitMacAddress, sizeof(submitMacAddress));
        socket->flush();
        socket->waitForBytesWritten(30000);
        submitStatus = SUBMIT_STATUS_LAN_DONE;
        emit submitStatusChange(SUBMIT_STATUS_LAN_DONE);
        break;
    case SUBMIT_STATUS_CPU_DONE:
    case SUBMIT_STATUS_LAN_DONE:
        socket->write("NODATA", 6);
        socket->flush();
        break;
    case SUBMIT_STATUS_PENDING:
        socket->write("DATAPENDING", 11);
        socket->flush();
        break;
    default:
        socket->write("NODATA", 6);
        socket->flush();
        break;
    }
    return 0;
}

void TcpServer::on_popupQueryFinished(int res) {
    this->setQueryResponseStatus(res);
    emit this->popupQueryMessageFinished();
}

int TcpServer::getQueryResponseStatus() {
    return this->dialogResponse;
}

void TcpServer::setQueryResponseStatus(int res) {
    this->dialogResponse = res;
}

void TcpServer::postTestResultData(QByteArray data) {
    emit postTestResults(data);
}

void TcpServer::postOverallResultData(QByteArray data) {
    emit postOverallTestResult(data);
}

void TcpServer::postSaveResultStatusData(QByteArray data) {
    emit postSaveResultStatus(data);
}

void TcpServer::popupStatusData(QByteArray data) {
    QString strMessage = QString::fromUtf8(data.remove(0,12));
    emit popupStatusMessage(strMessage);
}

void TcpServer::popupStatusClose() {
    emit popupStatusMessageClose();
}

void TcpServer::popupQueryData(QByteArray data) {
    QString strMessage = QString::fromUtf8(data.remove(0,11));
    emit popupQueryMessage(strMessage);
}

void TcpServer::showTestProgressData(QByteArray data) {
    QString strMessage = QString::fromUtf8(data.remove(0,13));
    emit showTestProgress(strMessage);
}

void TcpServer::gotoCpuPageSwitch() {
    emit gotoCpuPageSwitchClicked();
}

void TcpServer::gotoLanPageSwitch() {
    emit gotoLanPageSwitchClicked();
}
