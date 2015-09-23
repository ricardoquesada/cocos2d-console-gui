#include <QPushButton>

#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::appendData(const QString& str)
{
    ui->textBrowser->append(str);
}

void ProgressDialog::processFinished()
{
    ui->progressBar->setMaximum(1);
    ui->progressBar->setValue(1);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
