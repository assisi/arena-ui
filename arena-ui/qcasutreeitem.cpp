#include "qcasutreeitem.h"

QCasuTreeItem::QCasuTreeItem(QObject* parent, QString name) : QObject(parent), casu_name(name), child_selected(false)
{
    this->setData(0,Qt::DisplayRole,QStringList(casu_name));
    widget_IR = new QTreeWidgetItem(QStringList("IR - Proximity"));
    widget_LED= new QTreeWidgetItem(QStringList("LED"));
    widget_temp = new QTreeWidgetItem(QStringList("Temperature"));
    widget_vibr = new QTreeWidgetItem(QStringList("Vibration"));
    widget_light = new QTreeWidgetItem(QStringList("Light"));

    //zadavanje djece IR grani:
    {
        widget_IR->addChild(new QTreeBuffer(QStringList("N"), name + ": IR - N"));
        widget_IR->addChild(new QTreeBuffer(QStringList("NE"), name + ": IR - NE"));
        widget_IR->addChild(new QTreeBuffer(QStringList("SE"), name + ": IR - SE"));
        widget_IR->addChild(new QTreeBuffer(QStringList("S"), name + ": IR - S"));
        widget_IR->addChild(new QTreeBuffer(QStringList("SW"), name + ": IR - SW"));
        widget_IR->addChild(new QTreeBuffer(QStringList("NW"), name + ": IR - NW"));
    }
    //zadavanje djece temp grani:
    {
        widget_temp->addChild(new QTreeBuffer(QStringList("N"), name + ": IR - N"));
        widget_temp->addChild(new QTreeBuffer(QStringList("E"), name + ": IR - E"));
        widget_temp->addChild(new QTreeBuffer(QStringList("S"), name + ": IR - S"));
        widget_temp->addChild(new QTreeBuffer(QStringList("W"), name + ": IR - W"));
    }
    //zadavanje djece vibr grani:
    {
        widget_vibr->addChild(new QTreeBuffer(QStringList("N"), name + ": IR - N"));
        widget_vibr->addChild(new QTreeBuffer(QStringList("E"), name + ": IR - E"));
        widget_vibr->addChild(new QTreeBuffer(QStringList("S"), name + ": IR - S"));
        widget_vibr->addChild(new QTreeBuffer(QStringList("W"), name + ": IR - W"));
    }

    QList<QTreeWidgetItem *> temp;{
        temp.push_back(widget_IR);
        temp.push_back(widget_LED);
        temp.push_back(widget_temp);
        temp.push_back(widget_vibr);
        temp.push_back(widget_light);
    }

    this->addChildren(temp);
    for(int k = 0; k < this->childCount(); k++)this->child(k)->setFlags(Qt::ItemIsEnabled);
    this->setFlags(Qt::ItemIsEnabled);

    led_on = false;
    connection_timer = new QTimer(this);

    context_ = createDefaultContext(this);
    context_->start();
    sub_sock_ = context_->createSocket(ZMQSocket::TYP_SUB, this);
    pub_sock_ = context_->createSocket(ZMQSocket::TYP_PUB, this);

    connect(sub_sock_, SIGNAL(messageReceived(const QList<QByteArray>&)), SLOT(messageReceived(const QList<QByteArray>&)));

    connect(connection_timer, SIGNAL(timeout()),SLOT(connectionTimeout()));

    connect(this->QObject::parent(), SIGNAL(itemSelectionChanged()), SLOT(widgetClicked()));
}

void QCasuTreeItem::resetSelection(){
    for(int k=0;k<this->childCount();k++)
        for(int i=0;i<this->child(k)->childCount();i++)
            this->child(k)->child(i)->setSelected(false);
}

void QCasuTreeItem::widgetClicked(){
    child_selected = false;
    for(int k=0;k<this->childCount();k++)
        for(int i=0;i<this->child(k)->childCount();i++) if(this->child(k)->child(i)->isSelected()) child_selected = true;

    emit updateScene();
}

void QCasuTreeItem::setAddr(QString sub, QString pub, QString msg){
    sub_addr = sub;
    pub_addr = pub;
    msg_addr = msg;

    this->connect_();
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

    if(log_on & !log_open) openLogFile();
    if(!log_on & log_open) closeLogFile();

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
            widget_IR->child(k)->setData(1, Qt::DisplayRole, QVariant(value));
            ((QTreeBuffer *)widget_IR->child(k))->addToBuffer(QTime::currentTime(), value);
            if(log_on) log_file << ";" << value;
        }
    }

    if (device == "Temp"){
        TemperatureArray temperatures;
        temperatures.ParseFromString(data);
        for (int k = 0; k < temperatures.temp_size()-1; k++){
            double value = lexical_cast <double>(temperatures.temp(k));
            widget_temp->child(k)->setData(1, Qt::DisplayRole, QVariant(value));
            ((QTreeBuffer *)widget_temp->child(k))->addToBuffer(QTime::currentTime(), value);
            if(log_on) log_file << ";" << value;
        }
    }
/*
    if (device == "Acc"){
        VibrationArray ranges;
        ranges.ParseFromString(data);
        for (int k = 0; k < ranges.range_size(); i++)
            widget_vibr->child(i)->setData(1, Qt::DisplayRole,
                                  QVariant(lexical_cast<string>(ranges.range(i)).c_str()));
    }

    if (device == "Peltier")
    {
        //Peltier code

    }

    if (device == "DiagnosticLed")
    {
        ColorStamped LEDcolor;
        LEDcolor.ParseFromString(data);
        if(command == "On") led_on = true;
        else led_on = false;
        //led_color =
    }
*/

    log_file << endl;

}

void QCasuTreeItem::connectionTimeout(){
    connected = false;
    emit updateScene();
    connection_timer->stop();
}

void QCasuTreeItem::openLogFile(){
    log_name = logSubFolder + QDateTime::currentDateTime().toString(date_time_format) + casu_name;
    log_file.open(log_name.toStdString().c_str(), ofstream::out | ofstream::app);
    log_open = true;
}

void QCasuTreeItem::closeLogFile(){
    log_file.close();
    log_open = false;
}

