#include "stdafx.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
    QApplication a(argc, argv);
    MainWindow w;
    QStringList args = a.arguments();
    if(args.contains("\\iadb_admin", Qt::CaseInsensitive))
    {
        w.setAdminAccess(true);
    }
    w.show();

    return a.exec();
}
