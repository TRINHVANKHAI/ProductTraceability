#include "dbcontroller.h"
#include <QDebug>

DbController::DbController(QObject* parent) :
    QThread(parent)
{

}

DbController::~DbController()
{
    if (db.isOpen())
        db.close();
    quit();
    wait();
}

void DbController::run() {
    exec();
}

void DbController::connectToServerRequested(QString engine, QString driver, QString server, int port, QString database,
                                            QString login, QString password, bool is_sql_authentication)
{
    db = QSqlDatabase();
    db.removeDatabase("host-connection"); // remove old connection if exists

    if (engine == "mysql")
    {
        db = QSqlDatabase::addDatabase("QMYSQL", "host-connection");
    }
    else if (engine == "mssql")
    {
        db = QSqlDatabase::addDatabase("QODBC", "host-connection");
    }
    else
    {
        emit serverErrorWithConnection("Unknown database engine");
        return;
    }

    bool connection_succesfull;

    if (engine == "mysql")
    {
        connection_succesfull = connectToServerMySQL(server, port, database, login, password);
    }
    else if (engine == "mssql")
    {
        connection_succesfull =
                (is_sql_authentication ? connectToServerMSSQL(driver, server, port, database, login, password) :
                                         connectToServerMSSQL(driver, server, port, database));
    }
    else
    {
        emit serverErrorWithConnection("Unknown database engine");
        return;
    }

    if (connection_succesfull)
        emit serverConnected();
    else
        emit serverErrorWithConnection(getLastError().driverText());
}

void DbController::disconnectFromServerRequested()
{
    disconnectFromServer();

    emit serverDisconnected();
}

bool DbController::checkIfTableExists(QString table)
{
    return db.tables().contains(table);
}

bool DbController::checkIfConnected()
{
    return db.isOpen();
}


bool DbController::connectToServerMSSQL(QString driver, QString server, int port, QString database,
                                   QString login, QString password)
{
    db.setDatabaseName(connection_string_sqlauth.arg(driver).arg(server).arg(port).arg(database)
                       .arg(login).arg(password));

    return db.open();
}

bool DbController::connectToServerMSSQL(QString driver, QString server, int port, QString database)
{
    db.setDatabaseName(connection_string_winauth.arg(driver).arg(server).arg(port).arg(database));

    return db.open();
}

bool DbController::connectToServerMySQL(QString server, int port, QString database,
                                        QString login, QString password)
{
    db.setHostName(server);
    db.setPort(port);
    db.setDatabaseName(database);
    db.setUserName(login);
    db.setPassword(password);

    return db.open();
}

void DbController::disconnectFromServer()
{
    db.close();
}


QSqlError DbController::getLastError()
{
    return db.lastError();
}

const QString DbController::connection_string_sqlauth =
        QString("DRIVER={%1};SERVER=%2;PORT=%3;DATABASE=%4;UID=%5;PWD=%6");

const QString DbController::connection_string_winauth =
        QString("DRIVER={%1};SERVER=%2;PORT=%3;DATABASE=%4");


int DbController::exportToCSVFile(QString table, QString csvExportDir, QDateTime beginDT, QDateTime endDT) {
    QSqlQuery query(db);
    QString colName = QString::fromUtf8("検査日時");

    int fIdx;
    QString fIdxString;
    QString csvFileNamePrefix;
    QString csvExportPath;
    QString currentDateTimeString = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString beginDateTimeString = beginDT.toString("yyyy/MM/dd");
    QString endDateTimeString = endDT.addDays(1).toString("yyyy/MM/dd");

    if(table.startsWith(QString("RDCA"))) {
        csvFileNamePrefix = QString("RDCA-B01_776884+_") + currentDateTimeString + QString("_A_");
    } else if (table.startsWith(QString("EUCU"))) {
        csvFileNamePrefix = QString("EUCU-A03_E33082+_") + currentDateTimeString + QString("_A_");
    } else {
        qDebug() << "Table name is not valid";
        return -1;
    }

    for (fIdx=1; fIdx<100; fIdx++) {
        fIdxString = QString("%1").arg(fIdx, 2, 10, QChar('0'));
        csvExportPath=csvExportDir+QString("/")+csvFileNamePrefix+fIdxString+QString(".csv");
        if(!QFileInfo::exists(csvExportPath)) {
            break;
        }
    }

    qDebug() << "CSV Export PATH: " << csvExportPath;
    qDebug() << "Begin: " << beginDateTimeString;
    qDebug() << "End: " << endDateTimeString;

    query.clear();
    query.prepare(QString("SELECT * FROM `"+table+"` ") + QString("WHERE `")+ colName + QString("` >= '")
                  + beginDateTimeString + QString("' AND `") + colName + QString("` < '")
                  + endDateTimeString   + QString("'"));
    if(!query.exec()) {
        qDebug() << "Error in executing query: " << query.lastError();
        return -1;
    } else {
        QFile exportFile(csvExportPath);
        if(exportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream exportcontents(&exportFile);
            exportcontents.setCodec("UTF-8");
            exportcontents.setGenerateByteOrderMark(true);
            int colIdx = 0;
            int colCount = query.record().count();
            for(colIdx=0; colIdx < colCount; colIdx++) {
                if(colIdx<colCount-1)
                    exportcontents << query.record().fieldName(colIdx).toUtf8() + QString(",");
                else
                    exportcontents << query.record().fieldName(colIdx).toUtf8();
            }
            exportcontents << QString("\n");
            while(query.next()) {
                for(colIdx=0; colIdx < colCount; colIdx++) {
                    if(colIdx<colCount-1)
                        exportcontents << query.value(colIdx).toString().toUtf8() + QString(",");
                    else
                        exportcontents << query.value(colIdx).toString().toUtf8();
                }
                exportcontents << QString("\n");
            }
            exportcontents.flush();
            exportFile.close();
        }
    }

    return 0;
}
