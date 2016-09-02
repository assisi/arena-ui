#include "qdialogsetpoint.h"
#include "ui_qdialogsetpoint.h"

QDialogSetpoint::QDialogSetpoint(QWidget *parent, QString command, QList<QGraphicsItem *> group) :
    QDialog(parent),
    ui(new Ui::QDialogSetpoint),
    _command(command)
{
    ui->setupUi(this);

    bool groupSelected;

    QCasuSceneItem *tempItem;

    if(group.size() > 1) groupSelected = true;
    else if(sCast(group.first())->isGroup()) groupSelected = true;
    else {
        groupSelected = false;
        tempItem = siCast(group.first());
    }

    this->setWindowTitle(command + " data to send to CASUs");

    auto validator1 = new QDoubleValidator;
    auto validator2 = new QDoubleValidator;
    validator1->setNotation(QDoubleValidator::StandardNotation);
    validator2->setNotation(QDoubleValidator::StandardNotation);

    if(command != "Vibration"){
        delete ui->text21;
        delete ui->text22;
        delete ui->value2;
    }
    if(command == "IR Proximity"){
        delete ui->text11;
        delete ui->text12;
        delete ui->value1;
    }
    if(command != "LED") delete ui->colorButton;

    if(command == "Temperature"){
        ui->text11->setText("Temperature setpoint:");
        ui->text12->setText("Allowed temperature range: [26,45]°C");

        validator1->setRange(26.0,45.0,2);
        ui->value1->setValidator(validator1);

        if(groupSelected) ui->value1->setText("26.00");
        else{
            double temp = tempItem->getZmqObject()->getValue(Peltier);
            if(temp > 26)
                ui->value1->setText(QString::number(temp,'f',2));
            else
                ui->value1->setText("26.00");
        }
    }

    if(command == "Vibration"){
        ui->text11->setText("Vibration frequency setpoint:");
        ui->text12->setText("Allowed frequency range: [50,1500]Hz");
        ui->text21->setText("Vibration amplitude setpoint:");
        ui->text22->setText("Allowed amplitude range: [0,50]\%");

        validator1->setRange(50.0,1500.0,2);
        ui->value1->setValidator(validator1);
        validator2->setRange(0.0,50.0,2);
        ui->value2->setValidator(validator2);

        if(groupSelected){
            ui->value1->setText("500.00");
            ui->value2->setText("50.00");
        }
        else{
            double temp1 = tempItem->getZmqObject()->getValue(Frequency);
            double temp2 = tempItem->getZmqObject()->getValue(Amplitude);

            if(temp1 >= 50){
                ui->value1->setText(QString::number(temp1,'f',2));
                ui->value1->setText(QString::number(temp2,'f',2));
            }
            else{
                ui->value1->setText("500.00");
                ui->value2->setText("50.00");
            }
        }
    }

    if(command == "LED"){
        ui->text11->setText("Color:");
        ui->text12->setText("[hex #rrggbb], [hex #RRGGBB]");
        QRegularExpression re("^#([A-Fa-f0-9]{6})$");
        ui->value1->setValidator(new QRegularExpressionValidator(re, this));
        ui->value1->setText("#7aba71");
    }

    if(command == "Airflow"){
        ui->text11->setText("Intensity setpoint:");
        ui->text12->setText("Allowed intensity range: 1 (value is discarded)");

        validator1->setRange(0.0,1.0,2);
        ui->value1->setValidator(validator1);
        ui->value1->setDisabled(true); // DISABLED FOR FUTURE UPDATES; CURENTLY THERE IS ONLY ONE INTESITY (1)

        if(groupSelected) ui->value1->setText("1.00");
        else{
            double temp = tempItem->getZmqObject()->getValue(Airflow);
            if(temp > 1)
                ui->value1->setText(QString::number(temp,'f',2));
            else
                ui->value1->setText("1.00");
        }
    }

    this->adjustSize();
}

QList<QByteArray> QDialogSetpoint::getMessage()
{
    return _message;
}

void QDialogSetpoint::prepareMessage()
{
    if (_command != "IR Proximity" && !ui->value1->hasAcceptableInput()){
        reject();
        return;
    }
    if (_command == "Vibration" && !ui->value2->hasAcceptableInput()){
            reject();
            return;
        }

    if(_command == "Temperature"){
        AssisiMsg::Temperature temp;
        temp.set_temp(ui->value1->text().toFloat());

        int size = temp.ByteSize();
        void *buffer = malloc(size);
        temp.SerializePartialToArray(buffer, size);

        _message.append(QString("Peltier").toLocal8Bit());
        _message.push_back(QString(ui->radioON->isChecked() ? "On" : "Off").toLocal8Bit());
        _message.push_back(QByteArray((char*)buffer, size));
    }
    if(_command == "Vibration"){
        AssisiMsg::VibrationSetpoint vibr;
        vibr.set_freq(ui->value1->text().toDouble());
        vibr.set_amplitude(ui->value2->text().toDouble());

        int size = vibr.ByteSize();
        void *buffer = malloc(size);
        vibr.SerializeToArray(buffer,size);

        _message.push_back(QString("Speaker").toLocal8Bit());
        _message.push_back(QString(ui->radioON->isChecked() ? "On" : "Off").toLocal8Bit());
        _message.push_back(QByteArray((char*)buffer, size));
    }
    if(_command == "LED"){
        QColor userColor(ui->value1->text());

        AssisiMsg::ColorStamped color;
        color.mutable_color()->set_red((double)userColor.red()/255);
        color.mutable_color()->set_green((double)userColor.green()/255);
        color.mutable_color()->set_blue((double)userColor.blue()/255);

        int size = color.ByteSize();
        void *buffer = malloc(size);
        color.SerializeToArray(buffer, size);

        _message.push_back(QString("DiagnosticLed").toLocal8Bit());
        _message.push_back(QString(ui->radioON->isChecked() ? "On" : "Off").toLocal8Bit());
        _message.push_back(QByteArray((char*)buffer, size));
    }
    if(_command == "Airflow"){
        AssisiMsg::Airflow air;
        air.set_intensity(ui->value1->text().toFloat());
        int size = air.ByteSize();
        void *buffer = malloc(size);
        air.SerializeToArray(buffer, size);

        _message.push_back(QString("Airflow").toLocal8Bit());
        _message.push_back(QString(ui->radioON->isChecked() ? "On" : "Off").toLocal8Bit());
        _message.push_back(QByteArray((char*) buffer,size));
    }

    if(_command == "IR Proximity"){
        void *buffer = malloc(sizeof(int));
        //air.SerializeToArray(buffer, sizeof(int));

        _message.push_back(QString("IR").toLocal8Bit());
        _message.push_back(QString(ui->radioON->isChecked() ? "Activate" : "Standby").toLocal8Bit());
        _message.push_back(QByteArray((char*) buffer,sizeof(int)));
    }

    accept();
}

void QDialogSetpoint::colorDialog()
{
    ui->value1->setText(QColorDialog::getColor(ui->value1->text(),this,"Choose color").name());
}
