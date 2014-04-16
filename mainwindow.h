#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "..\iadatabase\headers\IADataBase.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual bool eventFilter(QObject *, QEvent *);
    void fillModels(QString);
protected slots:
//    void moveSplitter(int pos, int index);

private slots:
    void on_pB3DModel_clicked();

private:
    Ui::MainWindow *ui;
    db::IADataBase iadb;
    QStandardItemModel * m_pImplantsTreeModel;
    QStandardItemModel * m_pAbutmentsTreeModel;
//    bool m_bLeftSplitterCatched;
//    bool m_bRightSplitterCatched;
};

#endif // MAINWINDOW_H
