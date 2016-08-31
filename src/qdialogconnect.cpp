#include "qdialogconnect.h"

QDialogConnect::QDialogConnect(QStringList addresses)
{
    this->setWindowTitle("CASU address");

    auto temp = new QGridLayout;
    _subAddr = new QLineEdit(addresses.at(0));
    _pubAddr = new QLineEdit(addresses.at(1));
    _msgAddr = new QLineEdit(addresses.at(2));

    temp->addWidget(new QLabel("Subscriber address:"),0,0);
    temp->addWidget(new QLabel("Publisher address:"),1,0);
    temp->addWidget(new QLabel("Message address:"),2,0);

    temp->addWidget(_subAddr,0,1);
    temp->addWidget(_pubAddr,1,1);
    temp->addWidget(_msgAddr,2,1);
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    temp->addWidget(buttons,3,1);
    this->setLayout(temp);

    QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialogConnect::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialogConnect::reject);
}

QStringList QDialogConnect::getAddresses()
{
    QStringList out;
    out.append(_subAddr->text());
    out.append(_pubAddr->text());
    out.append(_msgAddr->text());
    return out;
}

