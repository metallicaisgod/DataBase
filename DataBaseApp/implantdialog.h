#ifndef IMPLANTDIALOG_H
#define IMPLANTDIALOG_H

#include <QDialog>
#include "..\iadatabase\headers\IADataBase.h"

namespace Ui {
class ImplantDialog;
}

class ImplantDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImplantDialog(QWidget *parent = 0);
    ~ImplantDialog();
    void setImplant(db::DbImplant * imp)
    {
        m_Implant = imp;
        //set = (bool)m_Implant;
    }

private slots:
    void enableButton();

    void on_lEName_textEdited(const QString &arg1);

    void on_lEArticle_textEdited(const QString &arg1);

    void on_lED1_textEdited(const QString &arg1);

    void on_lED2_textEdited(const QString &arg1);

    void on_lEL1_textEdited(const QString &arg1);

    void on_lEL2_textEdited(const QString &arg1);

    void on_lELe_textEdited(const QString &arg1);

protected:
    void showEvent(QShowEvent * ev);
    void hideEvent(QHideEvent * ev);
private:
    Ui::ImplantDialog *ui;
    db::DbImplant * m_Implant;
   // bool set;
};

#endif // IMPLANTDIALOG_H
