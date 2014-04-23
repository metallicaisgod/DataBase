#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "treemodel.h"
#include "tablemodel.h"
#include "..\iadatabase\headers\IADataBase.h"

Q_DECLARE_METATYPE(db::DbSeries*)

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
    void fillModels();
    void closeEvent(QCloseEvent * ev);
    void showEvent(QShowEvent * ev);

protected slots:
//    void moveSplitter(int pos, int index);

private slots:
    void on_pB3DModel_clicked();

    void on_tVImplants_clicked(const QModelIndex &index);

    void on_tVImplants_collapsed(const QModelIndex &index);

    void tableImplantsStateChanged();

    void treeImplantStateChanged(QModelIndex);

    void on_tVAbutments_clicked(const QModelIndex &index);

    void on_tVAbutments_collapsed(const QModelIndex &index);

    void tableAbutmentsStateChanged();

    void treeAbutmentsStateChanged(QModelIndex);

    void on_actionAdd_Provider_triggered();

private:
    Ui::MainWindow *ui;
    db::IADataBase iadb;
    //QStandardItemModel * m_pImplantsTreeModel;
    //QStandardItemModel * m_pAbutmentsTreeModel;
    TreeModel * m_pImpTreeModel;
    TableModel * m_pImpTableModel;
    TreeModel * m_pAbutTreeModel;
    TableModel * m_pAbutTableModel;
    //QList<db::DbProvider*> providerList;
    QString fileName;
    int m_openGLwidth;
//    bool m_bLeftSplitterCatched;
//    bool m_bRightSplitterCatched;
};

#endif // MAINWINDOW_H
