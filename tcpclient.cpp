#include "tcpclient.h"
#include "tcpserver.h"
#include "userinfo.h"

#include <QDebug>

TcpClient::TcpClient(qintptr tid, QObject *parent) :
    QThread(parent)
{

    socketDescriptor = tid;
    qDebug() << "CLIENT::" << socketDescriptor << " Initialized";

    socket = new QTcpSocket(this);
    if(!socket->setSocketDescriptor(socketDescriptor)) {
        qDebug() << "CLIENT::" << socketDescriptor << " setSocketDescriptor Error";
        emit error(socket->error());
        return;
    }
    connect(socket, SIGNAL(connected()), this, SLOT(on_ClientConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(on_ClientDisconnected()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(on_ClientBytesWritten(qint64)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(on_ClientReadyRead()));

}

TcpClient::~TcpClient() {
    qDebug() << "CLIENT::" << socketDescriptor << " Deinit";
    quit();
    wait();

    delete socket;
    socketDescriptor = 0;

}

void TcpClient::run() {
    qDebug() << "CLIENT::" << socketDescriptor << " Started";

    exec();
}

void TcpClient::on_ClientConnected() {
    qDebug() << "CLIENT::" << socketDescriptor << " Connected";
}

void TcpClient::on_ClientDisconnected() {
    qDebug() << "CLIENT::" << socketDescriptor << "Disconnected";
    quit();
}

void TcpClient::on_ClientBytesWritten(qint64 bytesWritten) {
    qDebug() << "CLIENT::" << socketDescriptor << "SEND >> " << bytesWritten;

}

void TcpClient::on_ClientReadyRead() {
    QByteArray readData = socket->readAll();
    qDebug() << "CLIENT::" << socketDescriptor << "RECV << " << readData;
    TcpServer *server = qobject_cast<TcpServer*>(this->parent());
    if(readData.startsWith("GETUSERINPUT")) {
        server->writeData(socket);
    } else if(readData.startsWith("GETUSERNAME")) {
        server->writeUserInfoData(socket);
    } else if(readData.startsWith("POSTRESULTS")) {
        server->postTestResultData(readData);
    } else if(readData.startsWith("OVERALLSTATUS")) {
        server->postOverallResultData(readData);
    } else if(readData.startsWith("RESULTSAVESTA")) {
        server->postSaveResultStatusData(readData);
    } else if(readData.startsWith("STATUSPOPUP")) {
        server->popupStatusData(readData);
    } else if(readData.startsWith("STATUSDIALOGCLR")) {
        server->popupStatusClose();
    } else if(readData.startsWith("QUERYPOPUP")) {
        server->popupQueryData(readData);
    } else if(readData.startsWith("SHOWTESTPROG")) {
        server->showTestProgressData(readData);
    } else if(readData.startsWith("QUERYGETRES")) {
        this->targetGetDialogResponse();
    } else if(readData.startsWith("GOTOCPUMENU")) {
        server->gotoCpuPageSwitch();
    } else if(readData.startsWith("GOTOLANMENU")) {
        server->gotoLanPageSwitch();
    } else if(readData.startsWith("EXIT") || readData.startsWith("QUIT")) {
        quit();
    } else {
        socket->write("NOTSUPPORTED", 12);
        socket->flush();
    }
}

void TcpClient::targetGetDialogResponse() {
    TcpServer *server = qobject_cast<TcpServer*>(this->parent());
    int dialogResponse = server->getQueryResponseStatus();
    if (dialogResponse == 1) {
        socket->write("ACCEPTED");
        socket->flush();
    } else if (dialogResponse == 0) {
        socket->write("REJECTED");
        socket->flush();
    } else {
        socket->write("NOTSUPPORTED");
        socket->flush();
    }
    server->setQueryResponseStatus(0xff);
}
