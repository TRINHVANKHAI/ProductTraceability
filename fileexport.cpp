#include "fileexport.h"
#include "ui_fileexport.h"

#include <QFileDialog>
#include <QDir>

#include <QDebug>

FileExport::FileExport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileExport)
{
    ui->setupUi(this);
    this->setWindowTitle("検査データをCSVファイル保存");
    ui->stackedWidget->setCurrentWidget(ui->exportOption);
    QTime lastTimeOfDay(23,59,59,999);

    endDateTime = QDateTime::currentDateTime();
    startDateTime = endDateTime.addDays(0);
    QDateTime lastDateTime = endDateTime;
    lastDateTime.setTime(lastTimeOfDay);
    ui->startDateEdit->setDateTime(startDateTime);
    ui->endDateEdit->setDateTime(endDateTime);

    ui->startDateEdit->setMaximumDateTime(lastDateTime);
    ui->endDateEdit->setMaximumDateTime(lastDateTime);

    connect(ui->endDateEdit, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(on_endDateTimeChanged(QDateTime)));
    connect(ui->startDateEdit, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(on_startDateTimeChanged(QDateTime)));

    dbController = new DbController(this);

    //widget->db
    connect(this, SIGNAL(connectToDB(QString,QString,QString,int,QString,QString,QString,bool)),
            dbController, SLOT(connectToServerRequested(QString,QString,QString,int,QString,QString,QString,bool)));
    connect(this, SIGNAL(disconnectFromDB()), dbController, SLOT(disconnectFromServerRequested()));

    //db->widget
    connect(dbController, SIGNAL(serverConnected()), this, SLOT(on_dbConnected()));
    connect(dbController, SIGNAL(serverDisconnected()), this, SLOT(on_dbDisconnected()));
    connect(dbController, SIGNAL(serverErrorWithConnection(QString)), this, SLOT(on_dbErrorWithConnection(QString)));

}

FileExport::~FileExport()
{
    delete dbController;
    delete ui;
}

void FileExport::exportDBInit(QString iEngine, QString iDriver, QString iServer, int iPort,
                                    QString iDatabase, QString iUserid,QString iPassword) {
    this->engine     = iEngine;
    this->driver     = iDriver;
    this->server     = iServer;
    this->port       = iPort;
    this->dbName     = iDatabase;
    this->userid     = iUserid;
    this->password   = iPassword;
}

void FileExport::connectToDBRequest(QString iEngine, QString iDriver, QString iServer, int iPort,
                                    QString iDatabase, QString iUserid,QString iPassword) {
    emit connectToDB(iEngine, iDriver, iServer, iPort, iDatabase, iUserid, iPassword, true);
}

void FileExport::disconnectFromDBRequest() {
    emit disconnectFromDB();
}

void FileExport::on_dbConnected () {
    qDebug() << "DB connected! ";
}

void FileExport::on_dbDisconnected() {
    qDebug() << "DB disconnected! ";
}

void FileExport::on_dbErrorWithConnection(QString err) {
    qDebug() << "DB connection error: " << err;
}


void FileExport::on_exportButton_clicked()
{

    if(ui->cpuDataExportRadioButton->isChecked()) {
        dbName = "miwa_CPU__RDCA";
        dbTableName = "RDCA-B01_776884+_general_A_01";
    } else if (ui->lanDataExportRadioButton->isChecked()) {
        dbName = "miwa_LAN__EUCU";
        dbTableName = "EUCU-A03_E33082+_general_A_01";
    } else {
        return;
    }
    connectToDBRequest(engine, driver, server, port, dbName, userid, password);
    int ret = dbController->exportToCSVFile(dbTableName, csvExportDirectory, startDateTime, endDateTime);
    if(ret==0) {
        qDebug() << "Export to file succeed:" << csvExportDirectory;
        this->accept();
    } else {
        qDebug() << "Export to file failed :" << csvExportDirectory;
        this->reject();
    }
    disconnectFromDBRequest();
}


void FileExport::on_cancelButton_clicked()
{
    this->reject();
}


void FileExport::on_csvSelectButton_clicked()
{
    csvExportDirectory = QFileDialog::getExistingDirectory(this, "試験結果CSVファイル保存ディレクトリを選択してください", QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(csvExportDirectory.isEmpty()) {
        csvExportDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    }
    ui->filenameLabel->setText(csvExportDirectory);
    ui->stackedWidget->setCurrentWidget(ui->exportPage);

}

void FileExport::on_startDateTimeChanged(QDateTime sdt) {
    startDateTime = sdt;
    ui->endDateEdit->setMinimumDateTime(startDateTime);
}

void FileExport::on_endDateTimeChanged(QDateTime edt) {
    endDateTime = edt;
    ui->startDateEdit->setMaximumDateTime(endDateTime);
}

void FileExport::on_csvCancelButton_clicked()
{
    this->reject();
}

