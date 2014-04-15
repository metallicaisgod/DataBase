#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

protected slots:
    void moveSplitter(int pos, int index);

private:
    Ui::MainWindow *ui;
    bool m_bLeftSplitterCatched;
    bool m_bRightSplitterCatched;
};

#endif // MAINWINDOW_H
