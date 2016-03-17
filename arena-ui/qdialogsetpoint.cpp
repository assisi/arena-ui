#include "qdialogsetpoint.h"

QDialogSetpoint::QDialogSetpoint(QString command) : command_(command)
{
    this->setWindowTitle(command + " data to send to CASUs");

    QGroupBox* on_off = new QGroupBox("Select actuator state");
    QGridLayout *tempLayout = new QGridLayout;

    radioON = new QRadioButton("ON");
    radioON->setChecked(true);
    tempLayout->addWidget(radioON,0,0);
    tempLayout->addWidget(new QRadioButton("OFF"),0,1);
    on_off->setLayout(tempLayout);

    tempLayout = new QGridLayout;
    tempLayout->addWidget(on_off,0,0,1,2);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    if(command == "Temperature"){
        tempLayout->addWidget(new QLabel("Temperature setpoint:"),1,0);
        value1 = new QLineEdit;
        value1->setValidator(new QDoubleValidator(0,99,2));
        tempLayout->addWidget(value1,1,1);
        tempLayout->addWidget(buttons,2,0);
    }

    if(command == "Vibration"){
        tempLayout->addWidget(new QLabel("Vibration frequency setpoint:"),1,0);
        tempLayout->addWidget(new QLabel("Vibration amplitude setpoint:"),2,0);
        value1 = new QLineEdit;
        value2 = new QLineEdit;
        value1->setValidator(new QDoubleValidator(0,99,2));
        value2->setValidator(new QDoubleValidator(0,99,2));
        tempLayout->addWidget(value1,1,1);
        tempLayout->addWidget(value2,2,1);
        tempLayout->addWidget(buttons,3,0);
    }

    this->setLayout(tempLayout);

    QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(prepareMessage()));
    QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

}

void QDialogSetpoint::prepareMessage()
{
    using namespace AssisiMsg;

    if(command_ == "Temperature"){
        Temperature temp;
        temp.set_temp(value1->text().toFloat());

        int size = temp.ByteSize();
        void *buffer = malloc(size);
        temp.SerializePartialToArray(buffer, size);

        message.append(QString("Peltier").toLocal8Bit());
        message.push_back(QString(radioON->isChecked() ? "temp" : "Off").toLocal8Bit());
        message.push_back(QByteArray((char*)buffer, size));
    }
    if(command_ == "Vibration"){
        VibrationSetpoint temp;
        temp.set_freq(value1->text().toDouble());
        temp.set_amplitude(value2->text().toDouble());

        int size = temp.ByteSize();
        void *buffer = malloc(size);

        message.push_back(QString("VibeMotor").toLocal8Bit());
        message.push_back(QString(radioON->isChecked() ? "On" : "Off").toLocal8Bit());
        message.push_back(QByteArray((char*)buffer, size));
    }

    accept();
}
