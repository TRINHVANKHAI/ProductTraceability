#ifndef DBCONTROLLER_H
#define DBCONTROLLER_H

#include <QThread>
#include <QString>
#include <QtSql>

class DbController : public QThread
{
    Q_OBJECT
public:
    explicit DbController(QObject*);
    ~DbController();
    void run();
    bool checkIfTableExists(QString);
    bool checkIfConnected();
    int exportToCSVFile(QString table, QString csvExportDir, QDateTime begin, QDateTime end);

public slots:
    void connectToServerRequested(QString, QString, QString, int, QString, QString, QString, bool);
    void disconnectFromServerRequested();


signals:
    void serverConnected();
    void serverErrorWithConnection(QString);
    void serverDisconnected();

private:
    bool connectToServerMSSQL(QString, QString, int, QString, QString, QString);
    bool connectToServerMSSQL(QString, QString, int, QString);
    bool connectToServerMySQL(QString, int, QString, QString, QString);
    void disconnectFromServer();

    QSqlError       getLastError();

    QSqlDatabase db;
    static const QString connection_string_sqlauth;
    static const QString connection_string_winauth;
};
#endif // DBCONTROLLER_H
