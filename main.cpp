#include <QApplication>
#include "mainmenu.h"
#include "globalapplication.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //GlobalApplication a(argc, argv);
    MainMenu w;
    w.show();
    return a.exec();
}
