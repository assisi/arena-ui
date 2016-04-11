#include "qdialogconnect.h"

QDialogConnect::QDialogConnect(QString sub, QString pub, QString msg)
{
    this->setWindowTitle("CASU address");

    QGridLayout* temp = new QGridLayout;
    sub_addr = new QLineEdit(sub);
    pub_addr = new QLineEdit(pub);
    msg_addr = new QLineEdit(msg);

    temp->addWidget(new QLabel("Subscriber address:"),0,0);
    temp->addWidget(new QLabel("Publisher address:"),1,0);
    temp->addWidget(new QLabel("Message address:"),2,0);

    temp->addWidget(sub_addr,0,1);
    temp->addWidget(pub_addr,1,1);
    temp->addWidget(msg_addr,2,1);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    temp->addWidget(buttons,3,1);
    this->setLayout(temp);

    QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

