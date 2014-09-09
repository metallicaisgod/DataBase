#ifndef ABUTMENTDIALOG_H
#define ABUTMENTDIALOG_H

#include <QDialog>
#include "..\iadatabase\headers\IADataBase.h"

namespace Ui {
class AbutmentDialog;
}

class AbutmentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AbutmentDialog(QWidget *parent = 0);
    ~AbutmentDialog();
    void setAbutment(db::DbAbutment * imp)
    {
        m_Abutment = imp;
        //set = (bool)m_Implant;
    }
    void setAdminAccess(bool ac)
    {
        m_admin_access = ac;
    }

private slots:
    void enableButton();
    void on_lEName_textEdited(const QString &arg1);

    void on_lEArticle_textEdited(const QString &arg1);

    void on_lEDa_textEdited(const QString &arg1);

    void on_lELa_textEdited(const QString &arg1);

    void on_lEAlfa_textEdited(const QString &arg1);

    void on_pBShowPallete_clicked();

    void on_lEModelPath_textEdited(const QString &arg1);

    void on_lEComp_val_textEdited(const QString &arg1);

protected:
    void showEvent(QShowEvent * ev);
    void hideEvent(QHideEvent * ev);
private:
    Ui::AbutmentDialog *ui;
    db::DbAbutment * m_Abutment;
    bool m_admin_access;
};

#endif // ABUTMENTDIALOG_H
