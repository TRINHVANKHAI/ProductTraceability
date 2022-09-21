#ifndef POPUPSTATUS_H
#define POPUPSTATUS_H

#include <QDialog>

namespace Ui {
class PopupStatus;
}

class PopupStatus : public QDialog
{
    Q_OBJECT

public:
    explicit PopupStatus(QWidget *parent = 0);
    ~PopupStatus();
    void setDialogMessage(QString msg);

private:
    Ui::PopupStatus *ui;
};

#endif // POPUPSTATUS_H
