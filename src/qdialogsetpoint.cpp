#include "qdialogsetpoint.h"

QDialogSetpoint::QDialogSetpoint(QString command) : command_(command)
{
    this->setWindowTitle(command + " data to send to CASUs");

    QGroupBox* on_off = new QGroupBox("Select device state");
    QGridLayout *tempLayout = new QGridLayout;

    QDoubleValidator* validator1 = new QDoubleValidator;
    validator1->setNotation(QDoubleValidator::StandardNotation);
    QDoubleValidator* validator2 = new QDoubleValidator;
    validator2->setNotation(QDoubleValidator::StandardNotation);

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
        tempLayout->addWidget(new QLabel("Allowed temperature range: [26,45]°C"),2,0);
        value1 = new QLineEdit;
        validator1->setRange(26.0,45.0,2);
        value1->setValidator(validator1);
        tempLayout->addWidget(value1,1,1);
        tempLayout->addWidget(buttons,3,0);
    }

    if(command == "Vibration"){
        tempLayout->addWidget(new QLabel("Vibration frequency setpoint:"),1,0);
        tempLayout->addWidget(new QLabel("Allowed frequency range: [50,1500]Hz"),2,0);
        tempLayout->addWidget(new QLabel("Vibration amplitude setpoint:"),3,0);
        tempLayout->addWidget(new QLabel("Allowed amplitude range: [0,50]\%"),4,0);
        value1 = new QLineEdit;
        value2 = new QLineEdit;
        validator1->setRange(50.0,1500.0,2);
        value1->setValidator(validator1);
        validator2->setRange(0.0,50.0,2);
        value2->setValidator(validator2);
        tempLayout->addWidget(value1,1,1);
        tempLayout->addWidget(value2,3,1);
        tempLayout->addWidget(buttons,5,0);
    }

    if(command == "LED"){
        tempLayout->addWidget(new QLabel("Color[hex #RrGgBb]:"),1,0);
        value1 = new QLineEdit("#7ABA71");
        QRegularExpression re("^#([A-Fa-f0-9]{6})$");
        value1->setValidator(new QRegularExpressionValidator(re, this));
        tempLayout->addWidget(value1,1,1,1,2);

        QIcon icon(":/images/icons/open_folder_yellow.png");
        QPushButton* chooseColor = new QPushButton();
        chooseColor->setIcon(icon);
        chooseColor->setFixedSize(25,25);

        connect(chooseColor,SIGNAL(clicked()),this,SLOT(colorDialog()));

        tempLayout->addWidget(chooseColor,1,2);
        tempLayout->addWidget(buttons,2,0);
    }

    if(command == "Airflow"){
        tempLayout->addWidget(new QLabel("Intensity setpoint:"),1,0);
        tempLayout->addWidget(new QLabel("Allowed intensity range: 1 (value is discarded)"),2,0);
        value1 = new QLineEdit;
        validator1->setRange(1.0,1.0,2);
        value1->setValidator(validator1);
        tempLayout->addWidget(value1,1,1);
        tempLayout->addWidget(buttons,3,0);
    }

    if(command == "LED"){
        tempLayout->addWidget(buttons,1,0);
    }

    if(command == "IR Proximity"){
        /* Allows only Activation/Standby of proximity sensors.  */
        tempLayout->addWidget(buttons,3,0);
    }

    this->setLayout(tempLayout);

    QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(prepareMessage()));
    QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

}

void QDialogSetpoint::prepareMessage()
{
    using namespace AssisiMsg;

    if (!value1->hasAcceptableInput()){
        reject();
        return;
    }
    if(value2)
        if (!value2->hasAcceptableInput()){
                reject();
                return;
            }

    if(command_ == "Temperature"){
        Temperature temp;
        temp.set_temp(value1->text().toFloat());

        int size = temp.ByteSize();
        void *buffer = malloc(size);
        temp.SerializePartialToArray(buffer, size);

        message.append(QString("Peltier").toLocal8Bit());
        message.push_back(QString(radioON->isChecked() ? "On" : "Off").toLocal8Bit());
        message.push_back(QByteArray((char*)buffer, size));
    }
    if(command_ == "Vibration"){
        VibrationSetpoint vibr;
        vibr.set_freq(value1->text().toDouble());
        vibr.set_amplitude(value2->text().toDouble());

        int size = vibr.ByteSize();
        void *buffer = malloc(size);
        vibr.SerializeToArray(buffer,size);

        message.push_back(QString("Speaker").toLocal8Bit());
        message.push_back(QString(radioON->isChecked() ? "On" : "Off").toLocal8Bit());
        message.push_back(QByteArray((char*)buffer, size));
    }
    if(command_ == "LED"){
        QColor userColor(value1->text());

        ColorStamped color;
        color.mutable_color()->set_red((double)userColor.red()/255);
        color.mutable_color()->set_green((double)userColor.green()/255);
        color.mutable_color()->set_blue((double)userColor.blue()/255);

        int size = color.ByteSize();
        void *buffer = malloc(size);
        color.SerializeToArray(buffer, size);

        message.push_back(QString("DiagnosticLed").toLocal8Bit());
        message.push_back(QString(radioON->isChecked() ? "On" : "Off").toLocal8Bit());
        message.push_back(QByteArray((char*)buffer, size));
    }
    if(command_ == "Airflow"){
        Airflow air;
        air.set_intensity(value1->text().toFloat());
        int size = air.ByteSize();
        void *buffer = malloc(size);
        air.SerializeToArray(buffer, size);

        message.push_back(QString("Airflow").toLocal8Bit());
        message.push_back(QString(radioON->isChecked() ? "On" : "Off").toLocal8Bit());
        message.push_back(QByteArray((char*) buffer,size));
    }

    if(command_ == "IR Proximity"){
        void *buffer = malloc(sizeof(int));
        //air.SerializeToArray(buffer, sizeof(int));

        message.push_back(QString("IR").toLocal8Bit());
        message.push_back(QString(radioON->isChecked() ? "Activate" : "Standby").toLocal8Bit());
        message.push_back(QByteArray((char*) buffer,sizeof(int)));
    }

    accept();
}

void QDialogSetpoint::colorDialog()
{
    value1->setText(QColorDialog::getColor(value1->text(),this,"Choose color").name());
}
