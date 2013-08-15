#include <QtGui/QApplication>
#include "attitudeindicator.h"

//#include "mainwindow.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AttitudeIndicator attInd;
//    MainWindow w;
//    w.show();
    attInd.show();
    return a.exec();
}
