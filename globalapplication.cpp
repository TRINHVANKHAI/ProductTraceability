#include "globalapplication.h"
#include <QDebug>


GlobalApplication::GlobalApplication(int &argc, char **argv):
    QApplication(argc, argv)
{

}

GlobalApplication::~GlobalApplication() {

}

void GlobalApplication::setWindowInstance(QWidget *window) {
    widget = window;
}

bool GlobalApplication::notify(QObject *obj, QEvent *event) {
    const QMetaObject *metaObj = obj->metaObject();
    QString className = metaObj->className();

    if(event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(keyEvent->key() == 16777220) {
            qDebug() << strinput;
            qDebug() << className;
            strinput.clear();
        } else {
            char ckey = keyEvent->key();
            QString str_ascii = QString(ckey);
            strinput += str_ascii;
        }
        //return true;
    }
    return QApplication::notify(obj, event);
}
