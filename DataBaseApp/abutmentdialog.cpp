#include "abutmentdialog.h"
#include "ui_abutmentdialog.h"

AbutmentDialog::AbutmentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AbutmentDialog),
    m_Abutment(NULL)
{
    ui->setupUi(this);
    setFixedSize(sizeHint());
    ui->lEDa->setValidator(new QDoubleValidator(0,100, 2, ui->lEDa));
    ui->lELa->setValidator(new QDoubleValidator(0,100, 2, ui->lELa));
    ui->lEAlfa->setValidator(new QDoubleValidator(0,100, 2, ui->lEAlfa));
}

AbutmentDialog::~AbutmentDialog()
{
    delete ui;
}

void AbutmentDialog::showEvent(QShowEvent * ev)
{
    if(m_Abutment)
    {
        ui->lEName->setText(QString(m_Abutment->name));
        ui->lEArticle->setText(QString(m_Abutment->artikul));
        ui->lEDa->setText(QString("%1").arg(m_Abutment->D1));
        ui->lELa->setText(QString("%1").arg(m_Abutment->L1));
        ui->lEAlfa->setText(QString("%1").arg(m_Abutment->Alpha));
    }
    QPushButton * but = ui->buttonBox->button(QDialogButtonBox::Ok);
    but->setEnabled(false);
    ev->accept();
}


void AbutmentDialog::hideEvent(QHideEvent * ev)
{
    m_Abutment = NULL;
    ev->accept();
}


void AbutmentDialog::enableButton()
{
    QPushButton * but = ui->buttonBox->button(QDialogButtonBox::Ok);
    but->setEnabled(true);
}

void AbutmentDialog::on_lEName_textEdited(const QString &arg1)
{
    if(m_Abutment)
    {
        strncpy(m_Abutment->name, arg1.toLocal8Bit().data(), NAME_SIZE);
        enableButton();
    }
}

void AbutmentDialog::on_lEArticle_textEdited(const QString &arg1)
{
    if(m_Abutment)
    {
        strncpy(m_Abutment->artikul, arg1.toLocal8Bit().data(), ARTIKUL_SIZE);
        enableButton();
    }
}

void AbutmentDialog::on_lEDa_textEdited(const QString &arg1)
{
    if(m_Abutment)
    {
        m_Abutment->D1 = arg1.toDouble();
        enableButton();
    }
}

void AbutmentDialog::on_lELa_textEdited(const QString &arg1)
{
    if(m_Abutment)
    {
        m_Abutment->L1 = arg1.toDouble();
        enableButton();
    }
}

void AbutmentDialog::on_lEAlfa_textEdited(const QString &arg1)
{
    if(m_Abutment)
    {
        m_Abutment->Alpha = arg1.toDouble();
        enableButton();
    }
}
