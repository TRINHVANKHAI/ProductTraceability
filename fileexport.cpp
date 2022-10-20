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


void FileExport::setExportFileName(QString filename) {
    csvExportFileName = filename;
}

void FileExport::on_exportButton_clicked()
{

    if(ui->cpuDataExportRadioButton->isChecked()) {
        dbName = "miwa_CPU__RDCA";
        dbTableName = "RDCA-B01_776884+_general_A_01";
        setExportFileName("RDCA-B01_776884+_general_A_01.csv");
    } else if (ui->lanDataExportRadioButton->isChecked()) {
        dbName = "miwa_LAN__EUCU";
        dbTableName = "EUCU-A03_E33082+_general_A_01";
        setExportFileName("EUCU-A03_E33082+_general_A_01.csv");
    } else {
        return;
    }
    connectToDBRequest(engine, driver, server, port, dbName, userid, password);
    int ret = dbController->exportToCSVFile(dbTableName, csvExportDirectory+tr("/")+csvExportFileName);
    if(ret==0)
    qDebug() << "Export to file :" << csvExportDirectory+tr("/")+csvExportFileName;
    this->accept();
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

