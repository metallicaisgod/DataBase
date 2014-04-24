#include "stdafx.h"
#include "implantdialog.h"
#include "ui_implantdialog.h"

ImplantDialog::ImplantDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImplantDialog),
    m_Implant(NULL)
{
    ui->setupUi(this);
    setFixedSize(sizeHint());
    ui->lED1->setValidator(new QDoubleValidator(0,100, 2, ui->lED1));
    ui->lED2->setValidator(new QDoubleValidator(0,100, 2, ui->lED2));
    ui->lEL1->setValidator(new QDoubleValidator(0,100, 2, ui->lEL1));
    ui->lEL2->setValidator(new QDoubleValidator(0,100, 2, ui->lEL2));
    ui->lELe->setValidator(new QDoubleValidator(0,100, 2, ui->lELe));
}

ImplantDialog::~ImplantDialog()
{
    delete ui;
}

void ImplantDialog::showEvent(QShowEvent * ev)
{
    if(m_Implant)
    {
        ui->lEName->setText(QString(m_Implant->name));
        ui->lEArticle->setText(QString(m_Implant->artikul));
        ui->lED1->setText(QString("%1").arg(m_Implant->D1));
        ui->lED2->setText(QString("%1").arg(m_Implant->D2));
        ui->lEL1->setText(QString("%1").arg(m_Implant->L1));
        ui->lEL2->setText(QString("%1").arg(m_Implant->L2));
        ui->lELe->setText(QString("%1").arg(m_Implant->Le));
    }
    QPushButton * but = ui->buttonBox->button(QDialogButtonBox::Ok);
    but->setEnabled(false);
    ev->accept();
}


void ImplantDialog::hideEvent(QHideEvent * ev)
{
    m_Implant = NULL;
    ev->accept();
}

void ImplantDialog::enableButton()
{
    QPushButton * but = ui->buttonBox->button(QDialogButtonBox::Ok);
    but->setEnabled(true);
}

void ImplantDialog::on_lEName_textEdited(const QString &arg1)
{
    if(m_Implant)
    {
        strncpy(m_Implant->name, arg1.toLocal8Bit().data(), NAME_SIZE);
        enableButton();
    }
}

void ImplantDialog::on_lEArticle_textEdited(const QString &arg1)
{
    if(m_Implant)
    {
        strncpy(m_Implant->artikul, arg1.toLocal8Bit().data(), ARTIKUL_SIZE);
        enableButton();
    }
}

void ImplantDialog::on_lED1_textEdited(const QString &arg1)
{
    if(m_Implant)
    {
        m_Implant->D1 = arg1.toDouble();
        enableButton();
    }
}

void ImplantDialog::on_lED2_textEdited(const QString &arg1)
{
    if(m_Implant)
    {
        m_Implant->D2 = arg1.toDouble();
        enableButton();
    }
}

void ImplantDialog::on_lEL1_textEdited(const QString &arg1)
{
    if(m_Implant)
    {
        m_Implant->L1 = arg1.toDouble();
        enableButton();
    }
}

void ImplantDialog::on_lEL2_textEdited(const QString &arg1)
{
    if(m_Implant)
    {
        m_Implant->L2 = arg1.toDouble();
        enableButton();
    }
}

void ImplantDialog::on_lELe_textEdited(const QString &arg1)
{
    if(m_Implant)
    {
        m_Implant->Le = arg1.toDouble();
        enableButton();
    }
}
