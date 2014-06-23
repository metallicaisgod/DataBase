#include "stdafx.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
