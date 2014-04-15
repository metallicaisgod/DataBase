#include "stdafx.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->splitterLeft->handle(1)->installEventFilter(this);
    ui->splitterRight->handle(1)->installEventFilter(this);
    connect(ui->splitterLeft, SIGNAL(splitterMoved(int,int)), SLOT(moveSplitter(int,int)));
    connect(ui->splitterRight, SIGNAL(splitterMoved(int,int)), SLOT(moveSplitter(int,int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{

        if(obj == ui->splitterLeft->handle(1))
        {
            if(ev->type() == QEvent::MouseButtonPress)
            {
                m_bLeftSplitterCatched = true;
                m_bRightSplitterCatched = false;
            }
            else if(ev->type() == QEvent::MouseButtonRelease)
            {
                m_bLeftSplitterCatched = false;
                m_bRightSplitterCatched = false;
            }
        }
        else if(obj == ui->splitterRight->handle(1))
        {
            if(ev->type() == QEvent::MouseButtonPress)
            {
                m_bLeftSplitterCatched = false;
                m_bRightSplitterCatched = true;
            }
            else if(ev->type() == QEvent::MouseButtonRelease)
            {
                m_bLeftSplitterCatched = false;
                m_bRightSplitterCatched = false;
            }
        }
    return QMainWindow::eventFilter(obj, ev);
}


void MainWindow::moveSplitter(int pos, int index)
{
    if(m_bLeftSplitterCatched)
    {
        ui->splitterRight->handle(index)->move(ui->splitterRight->handle(index)->x(), pos);
        QRect rect = ui->tabVImplants->geometry();
        rect.setHeight(pos);
        ui->tabVImplants->setGeometry(rect);
        rect = ui->tabVAbutments->geometry();
        rect.setTop(ui->tabVImplants->height() + ui->splitterRight->handleWidth());
        ui->tabVAbutments->setGeometry(rect);
    }
    else if(m_bRightSplitterCatched)
    {
        ui->splitterLeft->handle(index)->move(ui->splitterLeft->handle(index)->x(), pos);
        QRect rect = ui->tVImplants->geometry();
        rect.setHeight(pos);
        ui->tVImplants->setGeometry(rect);
        rect = ui->tVAbutments->geometry();
        rect.setTop(ui->tVImplants->height() + ui->splitterLeft->handleWidth());
        ui->tVAbutments->setGeometry(rect);
    }
}
