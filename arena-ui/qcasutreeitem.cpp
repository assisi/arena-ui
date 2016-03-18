#include "qcasutreeitem.h"

QCasuTreeItem::QCasuTreeItem(QObject* parent, QString name) : QObject(parent), casu_name(name), child_selected(false)
{
    this->setData(0,Qt::DisplayRole,QStringList(casu_name));
    widget_IR = new QTreeWidgetItem(QStringList("IR - Proximity"));
    widget_LED= new QTreeWidgetItem(QStringList("LED"));
    widget_temp = new QTreeWidgetItem(QStringList("Temperature"));
    widget_vibr = new QTreeWidgetItem(QStringList("Vibration"));
    widget_setpoints = new QTreeWidgetItem(QStringList("Current setpoints"));

    //zadavanje djece IR grani:
    {
        widget_IR_children.append((QTreeWidgetItem*)new QTreeBuffer(QStringList("N"), name + ": IR - N"));
        widget_IR_children.append((QTreeWidgetItem*)new QTreeBuffer(QStringList("NE"), name + ": IR - NE"));
        widget_IR_children.append((QTreeWidgetItem*)new QTreeBuffer(QStringList("SE"), name + ": IR - SE"));
        widget_IR_children.append((QTreeWidgetItem*)new QTreeBuffer(QStringList("S"), name + ": IR - S"));
        widget_IR_children.append((QTreeWidgetItem*)new QTreeBuffer(QStringList("SW"), name + ": IR - SW"));
        widget_IR_children.append((QTreeWidgetItem*)new QTreeBuffer(QStringList("NW"), name + ": IR - NW"));
        widget_IR->addChildren(widget_IR_children);
    }

    //zadavanje djece temp grani:
    {
        widget_temp_children.append((QTreeWidgetItem*)new QTreeBuffer(QStringList("N"), name + ": IR - N"));
        widget_temp_children.append((QTreeWidgetItem*)new QTreeBuffer(QStringList("E"), name + ": IR - E"));
        widget_temp_children.append((QTreeWidgetItem*)new QTreeBuffer(QStringList("S"), name + ": IR - S"));
        widget_temp_children.append((QTreeWidgetItem*)new QTreeBuffer(QStringList("W"), name + ": IR - W"));
        widget_temp->addChildren(widget_temp_children);
    }
    //zadavanje djece vibr grani:
    {
        widget_vibr_children.append((QTreeWidgetItem*)new QTreeBuffer(QStringList("Frequency"), name + ": Vibration - freq"));
        widget_vibr_children.append((QTreeWidgetItem*)new QTreeBuffer(QStringList("Amplitude"), name + ": Vibration - amp"));
        widget_vibr_children.append((QTreeWidgetItem*)new QTreeBuffer(QStringList("StdDev"), name + ": Vibration - stdDev"));
        widget_vibr_children[2]->setDisabled(true);
        widget_vibr->addChildren(widget_vibr_children);
    }
    //zadavanje djece setpoint grani:
    {
        widget_setpoints_children.append(new QTreeWidgetItem(QStringList("Peltier")));
        widget_setpoints_children.append(new QTreeWidgetItem(QStringList("Airflow")));
        widget_setpoints_children.append(new QTreeWidgetItem(QStringList("Speaker")));
        widget_setpoints_vibr_children.append(new QTreeWidgetItem(QStringList("Frequency")));
        widget_setpoints_vibr_children.append(new QTreeWidgetItem(QStringList("Amplitude")));
        widget_setpoints_vibr_children[0]->setFlags(Qt::ItemIsEnabled);
        widget_setpoints_vibr_children[1]->setFlags(Qt::ItemIsEnabled);
        widget_setpoints_children[2]->addChildren(widget_setpoints_vibr_children);
        widget_setpoints->addChildren(widget_setpoints_children);
    }

    QList<QTreeWidgetItem *> temp;{
        temp.push_back(widget_IR);
        temp.push_back(widget_LED);
        temp.push_back(widget_temp);
        temp.push_back(widget_vibr);
        temp.push_back(widget_setpoints);
    }

    this->addChildren(temp);
    for(int k = 0; k < this->childCount(); k++)this->child(k)->setFlags(Qt::ItemIsEnabled);
    for(int k = 0; k < widget_setpoints->childCount(); k++)widget_setpoints->child(k)->setFlags(Qt::ItemIsEnabled);
    this->setFlags(Qt::ItemIsEnabled);

    led_on = false;
    connection_timer = new QTimer(this);

    context_ = createDefaultContext(this);
    context_->start();
    sub_sock_ = context_->createSocket(ZMQSocket::TYP_SUB, this);
    pub_sock_ = context_->createSocket(ZMQSocket::TYP_PUB, this);

    connect(sub_sock_, SIGNAL(messageReceived(const QList<QByteArray>&)), SLOT(messageReceived(const QList<QByteArray>&)));

    connect(connection_timer, SIGNAL(timeout()),SLOT(connectionTimeout()));

    connect(this->QObject::parent(), SIGNAL(itemSelectionChanged()), SLOT(updateSelection()));
}

void QCasuTreeItem::resetSelection(){
    for(int k=0;k<this->childCount();k++)
        for(int i=0;i<this->child(k)->childCount();i++)
            this->child(k)->child(i)->setSelected(false);
}

void QCasuTreeItem::updateSelection(){
    child_selected = false;
    for(int k=0;k<this->childCount() - 1;k++) // LAST CHILD ARE SETPOINTS, NO NEED TO CHECK SELECTION
        for(int i=0;i<this->child(k)->childCount();i++) if(this->child(k)->child(i)->isSelected()) child_selected = true;

    emit updateScene();
}

void QCasuTreeItem::setAddr(QString sub, QString pub, QString msg){
    sub_addr = sub;
    pub_addr = pub;
    msg_addr = msg;

    this->connect_();
}

bool QCasuTreeItem::sendSetpoint(QList<QByteArray> message)
{
    if(!connected) return false;
    message.push_front(QString(casu_name).toLocal8Bit());
    return pub_sock_->sendMessage(message);
}

void QCasuTreeItem::connect_()
{
    try{
        pub_sock_->connectTo(pub_addr);
        // Subscribe to everything!
        sub_sock_->subscribeTo("casu");
        sub_sock_->connectTo(sub_addr);
        connected = true;
        connection_timer->start(1000);
    }
    catch(zmq::error_t e){
        connected = false;
    }
}

void QCasuTreeItem::messageReceived(const QList<QByteArray>& message){
    if(!connected){
        connected = true;
        emit updateScene();
    }
    connection_timer->start(2000);

    if(settings->value("log_on").toBool() & !log_open) openLogFile();
    if(!settings->value("log_on").toBool() & log_open) closeLogFile();

    string name(message.at(0).constData(), message.at(0).length());
    string device(message.at(1).constData(), message.at(1).length());
    string command(message.at(2).constData(), message.at(2).length());
    string data(message.at(3).constData(), message.at(3).length());

    log_file << device << ";" << QDateTime::currentDateTime().toString(time_format).toStdString();

    if (device == "IR"){
        RangeArray ranges;
        ranges.ParseFromString(data);
        for (int k = 0; k < ranges.range_size()-1; k++){
            double value = lexical_cast<double>(ranges.range(k));
            widget_IR_children[k]->setData(1, Qt::DisplayRole, QVariant(value));
            ((QTreeBuffer *)widget_IR_children[k])->addToBuffer(QTime::currentTime(), value);
            if(settings->value("log_on").toBool()) log_file << ";" << value;
        }
    }

    if (device == "Temp"){
        TemperatureArray temperatures;
        temperatures.ParseFromString(data);
        for (int k = 0; k < temperatures.temp_size()-1; k++){
            double value = lexical_cast <double>(temperatures.temp(k));
            widget_temp_children[k]->setData(1, Qt::DisplayRole, QVariant(value));
            ((QTreeBuffer *)widget_temp_children[k])->addToBuffer(QTime::currentTime(), value);
            if(settings->value("log_on").toBool()) log_file << ";" << value;
        }
    }
/* TREBA JOS DOKUCITI GDJE KO KOGA
    if (device == "Acc"){
        VibrationReading vibr;
        vibr.ParseFromString(data);
        double value1 = vibr.freq();
        double value2 = vibr.amplitude();
        double value3 = vibr.amplitude_stdev();
        widget_vibr->child(0)->setData(1, Qt::DisplayRole, value1);
        widget_vibr->child(1)->setData(1, Qt::DisplayRole, value2);
        widget_vibr->child(2)->setData(1, Qt::DisplayRole, value3);
        if(settings->value("log_on").toBool()) log_file << ";" << value1 << ";" << value2;
    }*/

   if (device == "Peltier"){
       Temperature pelt;
       pelt.ParseFromString(data);
       double value = pelt.temp();
       widget_setpoints_children[0]->setData(1, Qt::DisplayRole, value);
       if(command == "On")widget_setpoints_children[0]->setTextColor(1, Qt::green);
       else widget_setpoints_children[0]->setTextColor(1, Qt::red);
       if(settings->value("log_on").toBool()) log_file << ";" << value;
    }

   if (device == "Airflow"){
       Airflow air;
       air.ParseFromString(data);
       double value = air.intensity();
       widget_setpoints_children[1]->setData(1, Qt::DisplayRole, value);
       if(command == "On")widget_setpoints_children[1]->setTextColor(1, Qt::green);
       else widget_setpoints_children[1]->setTextColor(1, Qt::red);
       if(settings->value("log_on").toBool()) log_file << ";" << value;
    }

   if (device == "Speaker"){
       VibrationSetpoint vibr;
       vibr.ParseFromString(data);
       double value1 = vibr.freq();
       double value2 = vibr.amplitude();
       widget_setpoints_vibr_children[0]->setData(1, Qt::DisplayRole, value1);
       widget_setpoints_vibr_children[1]->setData(1, Qt::DisplayRole, value2);
       if(command == "On"){
           widget_setpoints_vibr_children[0]->setTextColor(1, Qt::green);
           widget_setpoints_vibr_children[1]->setTextColor(1, Qt::green);
       }
       else {
           widget_setpoints_vibr_children[0]->setTextColor(1, Qt::red);
           widget_setpoints_vibr_children[1]->setTextColor(1, Qt::red);
       }
       if(settings->value("log_on").toBool()) log_file << ";" << value1 << ";" << value2;
    }

    if (device == "DiagnosticLed")
    {
        ColorStamped LEDcolor;
        LEDcolor.ParseFromString(data);
        QColor color(LEDcolor.color().red()*255,
                     LEDcolor.color().green()*255,
                     LEDcolor.color().blue()*255, 255);

        if(command == "On"){
            widget_LED->setData(1 ,Qt::DisplayRole, color.name());
            if(!led_on || led_color != color){
                led_on = true;
                led_color = color;
                emit updateScene();
                widget_LED->setTextColor(1, color);
            }
        }
        else{
            widget_LED->setData(1 ,Qt::DisplayRole, "");
            if(led_on){
                led_on = false;
                emit updateScene();
                widget_LED->setTextColor(1, Qt::black);
            }
        }
        if(settings->value("log_on").toBool()) log_file << ";" << color.name().toStdString();
    }

    log_file << endl;

}

void QCasuTreeItem::connectionTimeout(){
    connected = false;
    emit updateScene();
    connection_timer->stop();
}

void QCasuTreeItem::openLogFile(){
    log_name = settings->value("logSubFolder").toString() + QDateTime::currentDateTime().toString(date_time_format) + casu_name;
    log_file.open(log_name.toStdString().c_str(), ofstream::out | ofstream::app);
    log_open = true;
}

void QCasuTreeItem::closeLogFile(){
    log_file.close();
    log_open = false;
}

