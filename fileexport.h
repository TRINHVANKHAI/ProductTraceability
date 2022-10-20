#ifndef FILEEXPORT_H
#define FILEEXPORT_H

#include <QDialog>

#include "dbcontroller.h"

namespace Ui {
class FileExport;
}

class FileExport : public QDialog
{
    Q_OBJECT

public:
    explicit FileExport(QWidget *parent = nullptr);
    ~FileExport();
    void setExportFileName(QString filename);
    void exportDBInit(QString, QString, QString, int, QString, QString, QString);

signals:
    void connectToDB(QString engine, QString driver, QString server, int port, QString database,
                                                QString login, QString password, bool is_sql_authentication);
    void disconnectFromDB();

public slots:
    void on_dbConnected();
    void on_dbErrorWithConnection(QString);
    void on_dbDisconnected();

private slots:
    void on_exportButton_clicked();

    void on_cancelButton_clicked();

    void on_csvSelectButton_clicked();

private:
    void connectToDBRequest(QString, QString, QString, int, QString, QString, QString);
    void disconnectFromDBRequest();
    QString csvExportDirectory;
    QString csvExportFileName;
    QString dbTableName;
    QString engine;
    QString driver;
    QString server;
    int port;
    QString dbName;
    QString userid;
    QString password;
    Ui::FileExport *ui;
    DbController *dbController;
};

#endif // FILEEXPORT_H
