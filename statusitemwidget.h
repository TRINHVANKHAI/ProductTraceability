#ifndef STATUSITEMWIDGET_H
#define STATUSITEMWIDGET_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class statusItemWidget;
}

class statusItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit statusItemWidget(QString statusNo, QString statusDesc, QString status, QListWidgetItem *item, QWidget *parent=0);
    ~statusItemWidget();

private:
    Ui::statusItemWidget *ui;
};

#endif // STATUSITEMWIDGET_H
