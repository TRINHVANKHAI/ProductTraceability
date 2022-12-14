#include "mainmenu.h"
#include "ui_mainmenu.h"

#include <QDebug>
#include <QIcon>

MainMenu::MainMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainMenu)
{
    ui->setupUi(this);

    this->setWindowTitle("Product Tracability");

    this->setWindowIcon(QIcon("ProductTraceability.ico"));
    this->currentPage = ui->homePage;
    ui->stackedWidget->setCurrentWidget(this->currentPage);

    ui->stackedWidget->installEventFilter(this);
    inputField = PT_INPUT_FIELD_NONE;
    overallTestStatus = 0;

    server = new TcpServer(this);

    userInfoForm = new UserInfo(this);
    userInfoForm->setWindowTitle("担当者姓名");
    connect(userInfoForm, SIGNAL(accepted()), this, SLOT(on_UserInfoChanged()));
    QString usrName = userInfoForm->getUserName();
    if(usrName.isNull() || usrName.isEmpty()) {
        userInfoForm->show();
        userInfoForm->raise();
        userInfoForm->activateWindow();
    } else {
        ui->userNameLabel->setText(usrName);
        server->on_UserNameDataSubmitted(usrName);
    }

    ui->statusbar->setStyleSheet("background-color:rgb(244,244,244);");
    connect(this, SIGNAL(submitCpuData(QString)), server, SLOT(on_CpuDataSubmitted(QString)));
    connect(this, SIGNAL(submitLanData(QString,QString)), server, SLOT(on_LanDataSubmitted(QString,QString)));
    connect(this, SIGNAL(submitUserNameData(QString)), server, SLOT(on_UserNameDataSubmitted(QString)));

    connect(server, SIGNAL(submitStatusChange(SUBMIT_STATUS_T)), this, SLOT(on_submitStatusChanged(SUBMIT_STATUS_T)));

    connect(server, SIGNAL(postTestResults(QByteArray)), this, SLOT(on_postTestResults(QByteArray)));
    connect(server, SIGNAL(postOverallTestResult(QByteArray)), this, SLOT(on_postOverallTestResult(QByteArray)));
    connect(server, SIGNAL(postSaveResultStatus(QByteArray)), this, SLOT(on_postSaveResultStatus(QByteArray)));

    connect(server, SIGNAL(popupQueryMessage(QString)), this, SLOT(on_popupQueryMessage(QString)));
    connect(server, SIGNAL(popupQueryMessageFinished()), this, SLOT(on_popupQueryMessageFinished()));
    connect(server, SIGNAL(popupStatusMessage(QString)), this, SLOT(on_popupStatusMessage(QString)));
    connect(server, SIGNAL(popupStatusMessageClose()), this, SLOT(on_popupStatusMessageClose()));

    connect(server, SIGNAL(showTestProgress(QString)), this, SLOT(on_showTestProgress(QString)));

    connect(server, SIGNAL(gotoCpuPageSwitchClicked()), this, SLOT(on_cpuButton_clicked()));
    connect(server, SIGNAL(gotoLanPageSwitchClicked()), this, SLOT(on_lanButton_clicked()));
}

MainMenu::~MainMenu()
{
    inputField = PT_INPUT_FIELD_NONE;
    inputValue.clear();
    delete userInfoForm;
    delete server;
    delete ui;

}

void MainMenu::on_cpuButton_clicked()
{
    inputField = PT_INPUT_FIELD_SERIAL_NUMBER;
    inputValue.clear();
    ui->cpuSerialIndicator->setStyleSheet("QLabel { background-color : rgb(217, 217, 219); color : black; }");
    ui->cpuSerialStatus->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; qproperty-alignment: 'AlignVCenter | AlignRight';}");
    ui->cpuSerialStatus->setText("");
    ui->cpuSerialInputField->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
    ui->cpuSerialInputField->setText("");
    ui->cpuSerialInputFrame->setStyleSheet("QFrame { border: 1px solid rgb(50,205,50); }");

    clearTestResults();
    this->currentPage = ui->cpuPage;
    ui->stackedWidget->setCurrentWidget(this->currentPage);

    ui->cpuOverallTestStatus->setText("");
    ui->cpuOverallTestStatus->setStyleSheet("QLabel { background-color : white; color : white; }");
    ui->cpuSaveResultStatus->setText("");
    ui->cpuSaveResultStatus->setStyleSheet("QLabel { background-color : white; color : white; }");
}

void MainMenu::on_lanButton_clicked()
{
    //pageStage = PT_PAGE_STAGE_LAN;
    inputField = PT_INPUT_FIELD_SERIAL_NUMBER;
    inputValue.clear();

    ui->lanSerialIndicator->setStyleSheet("QLabel { background-color : rgb(217, 217, 219); color : black; }");
    ui->lanSerialStatus->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; qproperty-alignment: 'AlignVCenter | AlignRight';}");
    ui->lanSerialStatus->setText("");
    ui->lanSerialInputField->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
    ui->lanSerialInputField->setText("");
    ui->lanSerialInputFrame->setStyleSheet("QFrame { border: 1px solid rgb(50,205,50); }");

    ui->lanMacIndicator->setStyleSheet("QLabel { background-color : rgb(217, 217, 219); color : black; }");
    ui->lanMacStatus->setStyleSheet("QLabel { background-color : rgb(240, 240, 245); color : black; qproperty-alignment: 'AlignVCenter | AlignRight';}");
    ui->lanMacStatus->setText("");
    ui->lanMacInputField->setStyleSheet("QLabel { background-color : rgb(240, 240, 245); color : black; }");
    ui->lanMacInputField->setText("");
    ui->lanMacInputFrame->setStyleSheet("QFrame { border: 0px solid rgb(119, 227, 237); }");

    clearTestResults();
    this->currentPage = ui->lanPage;
    ui->stackedWidget->setCurrentWidget(this->currentPage);

    ui->lanOverallTestStatus->setText("");
    ui->lanOverallTestStatus->setStyleSheet("QLabel { background-color : white; color : white; }");
    ui->lanSaveResultStatus->setText("");
    ui->lanSaveResultStatus->setStyleSheet("QLabel { background-color : white; color : white; }");
}

void MainMenu::on_cpuGoHome_clicked()
{
    inputField = PT_INPUT_FIELD_NONE;
    clearTestResults();
    this->currentPage = ui->homePage;
    ui->stackedWidget->setCurrentWidget(this->currentPage);

}

void MainMenu::on_lanGoHome_clicked()
{
    inputField = PT_INPUT_FIELD_NONE;
    clearTestResults();
    this->currentPage = ui->homePage;
    ui->stackedWidget->setCurrentWidget(this->currentPage);

}

bool MainMenu::eventFilter(QObject *obj, QEvent *event) {
    if(event->type() == QEvent::KeyPress) {
        eventKeyPressed(static_cast<QKeyEvent *>(event));
    }
    return false;
}

void MainMenu::eventKeyPressed(QKeyEvent* event)
{

    if(this->currentPage == ui->lanPage) {
        switch(inputField) {
        case PT_INPUT_FIELD_SERIAL_NUMBER:
            inputFieldShow = ui->lanSerialInputField;
            break;
        case PT_INPUT_FIELD_MAC_ADDRESS:
            inputFieldShow = ui->lanMacInputField;
            break;
        case PT_INPUT_FIELD_NONE:
            inputFieldShow = nullptr;
            break;
        default:
            inputFieldShow = nullptr;
            break;
        }
    } else if (this->currentPage ==  ui->cpuPage) {
        switch(inputField) {
        case PT_INPUT_FIELD_SERIAL_NUMBER:
            inputFieldShow = ui->cpuSerialInputField;
            break;
        case PT_INPUT_FIELD_MAC_ADDRESS:
            inputFieldShow = nullptr;
            break;
        case PT_INPUT_FIELD_NONE:
            inputFieldShow = nullptr;
            break;
        default:
            inputFieldShow = nullptr;
            break;
        }
    } else {
        inputFieldShow = nullptr;
    }

    switch(event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        clearTestResults();
        if(this->currentPage == ui->lanPage) {
            if ((inputField == PT_INPUT_FIELD_SERIAL_NUMBER) && serialNumberFormatCheck(inputValue, QString("EUCU"), 30)) {
                serialNumber = inputValue;
                ui->lanSerialStatus->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
                ui->lanSerialStatus->setText(inputValue);
                ui->lanSerialInputField->setStyleSheet("QLabel { background-color : rgb(240, 240, 245); color : black; }");
                ui->lanSerialInputField->setText("");
                ui->lanSerialInputFrame->setStyleSheet("QFrame { border: 0px solid rgb(240, 240, 245); }");

                ui->lanMacInputField->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
                ui->lanMacInputField->setText("");
                ui->lanMacInputFrame->setStyleSheet("QFrame { border: 1px solid rgb(50,205,50); }");

                macAddress.clear();
                ui->lanMacStatus->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
                ui->lanMacStatus->setText("");
                inputValue.clear();
                inputField = PT_INPUT_FIELD_MAC_ADDRESS;

            } else if ((inputField == PT_INPUT_FIELD_SERIAL_NUMBER) && (!serialNumberFormatCheck(inputValue, QString("EUCU"), 30))) {
                qDebug() << "LAN:: Error SN : " << inputValue;
                ui->lanSerialStatus->setStyleSheet("QLabel { background-color : yellow; color : red; }");
                ui->lanSerialStatus->setText("シリアル番号入力エラー");
                ui->lanSerialInputField->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
                ui->lanSerialInputField->setText("");
                ui->lanSerialInputFrame->setStyleSheet("QFrame { border: 1px solid rgb(50,205,50); }");

                macAddress.clear();
                ui->lanMacStatus->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
                ui->lanMacStatus->setText("");

                inputValue.clear();
                inputField = PT_INPUT_FIELD_SERIAL_NUMBER;

            } else if ((inputField == PT_INPUT_FIELD_MAC_ADDRESS) && macAddressFormatCheck(inputValue, 5)) {
                macAddress = inputValue;

                emit submitLanData(serialNumber, macAddress);

                ui->lanMacStatus->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
                ui->lanMacStatus->setText(inputValue);
                ui->lanMacInputField->setStyleSheet("QLabel { background-color : rgb(240, 240, 245); color : black; }");
                ui->lanMacInputField->setText("");
                ui->lanMacInputFrame->setStyleSheet("QFrame { border: 0px solid rgb(240, 240, 245); }");

                ui->lanSerialInputField->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
                ui->lanSerialInputField->setText("");
                ui->lanSerialInputFrame->setStyleSheet("QFrame { border: 1px solid rgb(50,205,50); }");
                inputValue.clear();
                inputField = PT_INPUT_FIELD_SERIAL_NUMBER;

            } else if ((inputField == PT_INPUT_FIELD_MAC_ADDRESS) && (!macAddressFormatCheck(inputValue, 5))) {
                qDebug() << "LAN:: Error MAC: " << inputValue;
                ui->lanMacStatus->setStyleSheet("QLabel { background-color : yellow; color : red; }");
                ui->lanMacStatus->setText("MACアドレス入力エラー");
                ui->lanMacInputField->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
                ui->lanMacInputField->setText("");
                ui->lanMacInputFrame->setStyleSheet("QFrame { border: 1px solid rgb(50,205,50); }");
                inputValue.clear();
                inputField = PT_INPUT_FIELD_MAC_ADDRESS;

            } else {
                qDebug() << "LAN:: Not implemented: " << inputValue << "size=" << inputValue.size();
                inputValue.clear();
            }

        } else if (this->currentPage == ui->cpuPage) {
            if ((inputField == PT_INPUT_FIELD_SERIAL_NUMBER) && serialNumberFormatCheck(inputValue, QString("RDCA"), 30)) {
                serialNumber = inputValue;
                emit submitCpuData(serialNumber);
                ui->cpuSerialStatus->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
                ui->cpuSerialStatus->setText(inputValue);
                ui->cpuSerialInputField->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
                ui->cpuSerialInputField->setText("");
                inputValue.clear();
                inputField = PT_INPUT_FIELD_SERIAL_NUMBER;
            } else if ((inputField == PT_INPUT_FIELD_SERIAL_NUMBER) && (!serialNumberFormatCheck(inputValue, QString("RDCA"), 30))) {
                qDebug() << "CPU:: Error SN : " << inputValue;
                ui->cpuSerialStatus->setStyleSheet("QLabel { background-color : yellow; color : red; }");
                ui->cpuSerialStatus->setText("無効な入力エラー");
                ui->cpuSerialInputField->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
                ui->cpuSerialInputField->setText("");
                inputValue.clear();
                inputField = PT_INPUT_FIELD_SERIAL_NUMBER;
            } else {
                qDebug() << "CPU:: Input value not valid: " << inputValue << "size=" << inputValue.size();
                inputValue.clear();
            }
        }
        break;

    default:
        bool asciiValid = event->key() >= Qt::Key_Space;
        asciiValid &= event->key() <= Qt::Key_ydiaeresis;
        if ((inputFieldShow!=nullptr) && (asciiValid)) {
            inputValue += QString(event->key());
            inputFieldShow->setText(inputValue);
        }
        break;
    }
}

int MainMenu::on_submitStatusChanged(SUBMIT_STATUS_T submitStatus) {
    switch(submitStatus) {
    case SUBMIT_STATUS_CPU_READY:
        ui->cpuSerialStatus->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
        break;

    case SUBMIT_STATUS_CPU_DONE:
        ui->cpuSerialStatus->setStyleSheet("QLabel { background-color : rgb(50,205,50); color : white; }");
        break;

    case SUBMIT_STATUS_LAN_READY:
        ui->lanSerialStatus->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
        ui->lanMacStatus->setStyleSheet("QLabel { background-color : rgb(255, 255, 255); color : black; }");
        break;

    case SUBMIT_STATUS_LAN_DONE:
        ui->lanSerialStatus->setStyleSheet("QLabel { background-color : rgb(50,205,50); color : white; }");
        ui->lanMacStatus->setStyleSheet("QLabel { background-color : rgb(50,205,50); color : white; }");
        break;

    default:
        break;
    }
    return 0;
}

bool MainMenu::serialNumberFormatCheck(QString serialNo, QString prefix, int length) {
    QString reString = QString("^") + prefix + QString("-[0-9A-Z]{5}(\\d){20}$");
    QRegularExpression re(reString, QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(serialNo);
    if (length != serialNo.size())
        return false;
    if (!match.hasMatch())
        return false;
    return true;
}

bool MainMenu::macAddressFormatCheck(QString macAddr, int length) {
    QString reString = QString("^[0-9A-F]{")+QString::number(length)+QString("}$");
    QRegularExpression re(reString, QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(macAddr);
    if(length != macAddr.size())
        return false;
    if (!match.hasMatch())
        return false;
    return true;
}

void MainMenu::clearTestResults() {
    ui->cpuListWidget->clear();
    ui->lanListWidget->clear();
    overallTestStatus=0;
    overallTestCount=0;
    ui->cpuOverallTestStatus->setText("");
    ui->cpuOverallTestStatus->setStyleSheet("QLabel { background-color : white; color : white; }");
    ui->lanOverallTestStatus->setText("");
    ui->lanOverallTestStatus->setStyleSheet("QLabel { background-color : white; color : white; }");
    ui->cpuSaveResultStatus->setText("");
    ui->cpuSaveResultStatus->setStyleSheet("QLabel { background-color : white; color : white; }");
    ui->lanSaveResultStatus->setText("");
    ui->lanSaveResultStatus->setStyleSheet("QLabel { background-color : white; color : white; }");
    ui->statusbar->showMessage("");
}

void MainMenu::showTestResult(QString statusNo, QString statusName, QString sts) {
    if(sts != "PASSED") {
        overallTestStatus++;
    }

    if (this->currentPage == ui->cpuPage) {
        cpuWidgetItem = new QListWidgetItem(ui->cpuListWidget);
        cpuWidgetItem->setSizeHint(QSize(500, 25));
        statusItemWidget *tempWidget = new statusItemWidget(statusNo, statusName, sts, cpuWidgetItem, this);
        ui->cpuListWidget->addItem(cpuWidgetItem);
        ui->cpuListWidget->setItemWidget(cpuWidgetItem, tempWidget);
    }
    if (this->currentPage == ui->lanPage) {
        lanWidgetItem = new QListWidgetItem(ui->lanListWidget);
        lanWidgetItem->setSizeHint(QSize(500, 25));
        statusItemWidget *tempWidget = new statusItemWidget(statusNo, statusName, sts, lanWidgetItem, this);
        ui->lanListWidget->addItem(lanWidgetItem);
        ui->lanListWidget->setItemWidget(lanWidgetItem, tempWidget);
    }
}

void MainMenu::on_postTestResults(QByteArray data) {
    overallTestCount++;
    QString strData = QString::fromUtf8(data.remove(0,12));
    strData.chop(1);
    QRegExp token(",");
    showTestResult(strData.split(token)[0],strData.split(token)[1], strData.split(token)[2]);
}

void MainMenu::on_postOverallTestResult(QByteArray data) {
    QString strData = QString::fromUtf8(data.remove(0,14));
    strData.chop(1);
    if (this->currentPage == ui->cpuPage) {
        if(strData=="0") {
            ui->cpuOverallTestStatus->setText("OK");
            ui->cpuOverallTestStatus->setStyleSheet("QLabel { background-color : green; color : white; }");
        } else if(strData=="1") {
            ui->cpuOverallTestStatus->setText("NG");
            ui->cpuOverallTestStatus->setStyleSheet("QLabel { background-color : red; color : white; }");
        }
    }

    if (this->currentPage == ui->lanPage) {
        if(strData=="0") {
            ui->lanOverallTestStatus->setText("OK");
            ui->lanOverallTestStatus->setStyleSheet("QLabel { background-color : green; color : white; }");
        } else if(strData=="1") {
            ui->lanOverallTestStatus->setText("NG");
            ui->lanOverallTestStatus->setStyleSheet("QLabel { background-color : red; color : white; }");
        }
    }
}

void MainMenu::on_postSaveResultStatus(QByteArray data) {
    QString strData = QString::fromUtf8(data.remove(0,14));
    strData.chop(1);
    if (this->currentPage == ui->cpuPage) {
        if(strData=="0") {
            ui->cpuSaveResultStatus->setText("OK");
            ui->cpuSaveResultStatus->setStyleSheet("QLabel { background-color : green; color : white; }");
        } else if(strData=="1") {
            ui->cpuSaveResultStatus->setText("NG");
            ui->cpuSaveResultStatus->setStyleSheet("QLabel { background-color : red; color : white; }");
        }
    }

    if (this->currentPage == ui->lanPage) {
        if(strData=="0") {
            ui->lanSaveResultStatus->setText("OK");
            ui->lanSaveResultStatus->setStyleSheet("QLabel { background-color : green; color : white; }");
        } else if(strData=="1") {
            ui->lanSaveResultStatus->setText("NG");
            ui->lanSaveResultStatus->setStyleSheet("QLabel { background-color : red; color : white; }");
        }
    }
}

void MainMenu::on_popupQueryMessage(QString msg) {
    if(popupQuery) {
        qDebug() << "MainMenu::Prevented multiple dialogs opening";
        delete popupQuery;
        this->popupQuery = nullptr;
    }

    popupQuery = new Dialog(this);
    popupQuery->setWindowTitle("Message");
    popupQuery->setQueryDialogMessage(msg);
    connect(popupQuery, SIGNAL(finished(int)), server, SLOT(on_popupQueryFinished(int)));
    popupQuery->show();
    popupQuery->raise();
    popupQuery->activateWindow();

}

void MainMenu::on_popupQueryMessageFinished() {
    if(popupQuery) {
        delete this->popupQuery;
        this->popupQuery = nullptr;
    }

    ui->stackedWidget->setCurrentWidget(this->currentPage);

}

void MainMenu::on_popupStatusMessage(QString msg) {
    if(popupStatus) {
        qDebug() << "MainMenu::Prevented multiple dialogs opening";
        delete popupStatus;
        this->popupStatus = nullptr;
    }
    popupStatus = new PopupStatus(this);
    popupStatus->setWindowTitle("Message");
    popupStatus->setDialogMessage(msg);
    popupStatus->show();
    popupStatus->raise();
    popupStatus->activateWindow();
}

void MainMenu::on_popupStatusMessageClose() {
    if(popupStatus) {
        delete this->popupStatus;
        this->popupStatus = nullptr;
    }
    ui->stackedWidget->setCurrentWidget(this->currentPage);

}

void MainMenu::on_showTestProgress(QString msg) {
    ui->statusbar->showMessage(msg);
}

void MainMenu::on_UserInfoChanged() {
    QString gUserName = userInfoForm->getUserName();
    ui->userNameLabel->setText(gUserName);
    emit submitUserNameData(gUserName);
    ui->stackedWidget->setCurrentWidget(this->currentPage);
}

void MainMenu::on_changeUserNameButton_clicked()
{
    userInfoForm->show();
    userInfoForm->raise();
    userInfoForm->activateWindow();
}

void MainMenu::on_actionOpen_triggered()
{
    ui->statusbar->showMessage("Open is not implemented yet");
}


void MainMenu::on_actionImport_triggered()
{
    ui->statusbar->showMessage("Import is not implemented yet");
}


void MainMenu::on_actionExport_triggered()
{

    QString engine     = "mysql";
    QString driver     = "ODBC";
    QString server     = "192.168.0.39";
    int port           = 3306;
    QString dbName     = "miwa_CPU__RDCA";
    QString userid     = "miwakensa";
    QString password   = "hinoeng";
    FileExport *exportWizard = new FileExport(this);
    connect(exportWizard, SIGNAL(finished(int)), this, SLOT(on_exportWizardFinished(int)));
    exportWizard->exportDBInit(engine, driver, server, port, dbName, userid, password);
    exportWizard->setAttribute(Qt::WA_DeleteOnClose);
    exportWizard->show();
    exportWizard->raise();
    exportWizard->isActiveWindow();
}

void MainMenu::on_exportWizardFinished(int flg) {
    ui->statusbar->showMessage("Export is finished ");
    ui->stackedWidget->setCurrentWidget(this->currentPage);
}

void MainMenu::on_actionSave_triggered()
{
    ui->statusbar->showMessage("Save is not implemented yet");
}

void MainMenu::on_actionExit_triggered()
{
    this->close();
}

