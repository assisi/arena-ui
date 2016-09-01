#include "qdialogconnect.h"
#include "ui_qdialogconnect.h"

QDialogConnect::QDialogConnect(QWidget *parent, QStringList addresses) :
    QDialog(parent),
    ui(new Ui::QDialogConnect)
{
    ui->setupUi(this);

    ui->subAddr->setText(addresses.at(0));
    ui->pubAddr->setText(addresses.at(1));
    ui->msgAddr->setText(addresses.at(2));
}

QDialogConnect::~QDialogConnect()
{
    delete ui;
}

QStringList QDialogConnect::getAddresses()
{
    QStringList out;
    out.append(ui->subAddr->text());
    out.append(ui->pubAddr->text());
    out.append(ui->msgAddr->text());
    return out;
}
