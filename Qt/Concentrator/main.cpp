#include "concentratorwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConcentratorWindow w;
    w.show();
    return a.exec();
}
