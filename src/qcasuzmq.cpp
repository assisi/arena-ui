#include "qcasuzmq.h"

QCasuZMQ::QCasuZMQ(QObject *parent, QString casuName) :
    QObject(parent),
    _name(casuName)
{
    for(int k = 0; k < _IR_num + _Temp_num; k++) _buffers.insert(static_cast<dataType>(k), new QCPDataMap());
    for(int k = _IR_num + _Temp_num; k < _dataType_num; k++)_state.insert(static_cast<dataType>(k), false);

    _connectionTimer = new QTimer(this);

    _context = createDefaultContext(this);
    _context->start();
    _subSock = _context->createSocket(ZMQSocket::TYP_SUB, this);
    _pubSock = _context->createSocket(ZMQSocket::TYP_PUB, this);

    connect(_subSock, SIGNAL(messageReceived(const QList<QByteArray>&)), SLOT(messageReceived(const QList<QByteArray>&)));
    connect(_connectionTimer, SIGNAL(timeout()),SLOT(connectionTimeout()));
}

QCPDataMap *QCasuZMQ::getBuffer(QCasuZMQ::dataType key)
{
    if (key < _IR_num + _Temp_num) return _buffers[key];
    return 0;
}

double QCasuZMQ::getValue(QCasuZMQ::dataType key)
{
    if (key < _IR_num + _Temp_num) return _buffers[key]->last().value;
    return _values[key].value;
}

QColor QCasuZMQ::getLedColor()
{
    return _ledColor;
}

bool QCasuZMQ::getState(QCasuZMQ::dataType key)
{
    return _state[key];
}

double QCasuZMQ::getAvgSamplingTime()
{
    double result = 0;
    double itemNum = 0;

    foreach(double data, _lastDataTime){
        dataType key = _lastDataTime.key(data, LED);
        if(key == LED) continue;
        result += _values[key].key - data;
        itemNum++;
    }
    /*check IR sampling time*/
    if(_lastDataTime.contains(static_cast<dataType>(0))){
        result += _buffers[static_cast<dataType>(0)]->lastKey() - _lastDataTime[static_cast<dataType>(0)];
        itemNum++;
    }
    /*check Temp sampling time*/{}
    if(_lastDataTime.contains(static_cast<dataType>(_IR_num))){
        result += _buffers[static_cast<dataType>(_IR_num)]->lastKey() - _lastDataTime[static_cast<dataType>(_IR_num)];
        itemNum++;
    }
    return itemNum && _connected ? result/itemNum : 0;
}

void QCasuZMQ::setAddress(QString sub, QString pub, QString msg)
{
    _subAddr = sub;
    _pubAddr = pub;
    _msgAddr = msg;

    this->connectZMQ();
}

bool QCasuZMQ::sendSetpoint(QList<QByteArray> message)
{
    if(!_connected) return false;
    message.push_front(QString(_name).toLocal8Bit());
    return _pubSock->sendMessage(message);
}

bool QCasuZMQ::isConnected()
{
    return _connected;
}

void QCasuZMQ::openLogFile()
{
    _logName = settings->value("logSubFolder").toString() + QDateTime::currentDateTime().toString(date_time_format) + _name + ".log";
    _logFile.open(_logName.toStdString().c_str(), ofstream::out | ofstream::app);
    _logOpen = true;
}

void QCasuZMQ::closeLogFile()
{
    _logFile.close();
    _logOpen = false;
}

void QCasuZMQ::addToBuffer(dataType key, QCPData data)
{
    _buffers[key]->insert(data.key, data);
    while(data.key - _buffers[key]->firstKey() > QTime(0,0,0).secsTo(settings->value("trendTimeSpan").toTime())) _buffers[key]->erase(_buffers[key]->begin()); //Delete data older than $timeSpan

    emit updated(key);
}

void QCasuZMQ::connectZMQ()
{
    try{
        _pubSock->connectTo(_pubAddr);
        // Subscribe to everything!
        _subSock->subscribeTo("casu");
        _subSock->connectTo(_subAddr);
        _connected = true;
        _connectionTimer->start(1000);
    }
    catch(zmq::error_t e){
        _connected = false;
    }
}

void QCasuZMQ::messageReceived(const QList<QByteArray> &message)
{
    string name(message.at(0).constData(), message.at(0).length());
    if(name != _name.toStdString()) return;
    if(!_connected) _connected = true;
    if(settings->value("log_on").toBool() & !_logOpen) openLogFile();
    if(!settings->value("log_on").toBool() & _logOpen) closeLogFile();

    _connectionTimer->start(2000);

    string device(message.at(1).constData(), message.at(1).length());
    string command(message.at(2).constData(), message.at(2).length());
    string data(message.at(3).constData(), message.at(3).length());

    QCPData newData;
    newData.key = (double) QTime(0,0,0).msecsTo(QTime::currentTime())/1000;

    _logFile << device << ";" << (float) QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000 ;

    if (device == "IR"){
        AssisiMsg::RangeArray ranges;
        ranges.ParseFromString(data);
        _lastDataTime[static_cast<dataType>(0)] = _buffers[static_cast<dataType>(0)]->lastKey();
        for (int k = 0; k < ranges.raw_value_size(); k++){
            if( k == _IR_num) break;
            newData.value = lexical_cast<double>(ranges.raw_value(k));
            this->addToBuffer(static_cast<dataType>(k), newData);
            if(settings->value("log_on").toBool()) _logFile << ";" << newData.value;
        }
    }
    if (device == "Temp"){
        AssisiMsg::TemperatureArray temperatures;
        temperatures.ParseFromString(data);
        _lastDataTime[static_cast<dataType>(_IR_num)] = _buffers[static_cast<dataType>(_IR_num)]->lastKey();
        for (int k = 0; k < temperatures.temp_size(); k++){
            if( k == _Temp_num) break;
            newData.value = lexical_cast <double>(temperatures.temp(k));
            this->addToBuffer(static_cast<dataType>(k), newData);
            if(settings->value("log_on").toBool()) _logFile << ";" << newData.value;
        }
    }
    if (device == "Peltier"){
        AssisiMsg::Temperature pelt;
        pelt.ParseFromString(data);
        newData.value = pelt.temp();
        _lastDataTime[Peltier] = _values[Peltier].key;
        _values[Peltier] = newData;
        _state[Peltier] = command == "On";
        if(settings->value("log_on").toBool()) _logFile << ";" << _values[Peltier].value
                                                       << ";" << _state[Peltier];
     }
    if (device == "Airflow"){
        AssisiMsg::Airflow air;
        air.ParseFromString(data);
        newData.value = air.intensity();
        _lastDataTime[Airflow] = _values[Airflow].key;
        _values[Airflow] = newData;
        _state[Airflow] = command == "On";
        if(settings->value("log_on").toBool()) _logFile << ";" << _values[Airflow].value
                                                       << ";" << _state[Airflow];

     }
    if (device == "Speaker"){
        AssisiMsg::VibrationSetpoint vibr;
        vibr.ParseFromString(data);
         _lastDataTime[Freq] = _values[Freq].key;
        newData.value = vibr.freq();
        _values[Freq] = newData;
        newData.value = vibr.amplitude();
        _values[Amplitude] = newData;
        _state[Speaker] = command == "On";
        if(settings->value("log_on").toBool()) _logFile << ";" << _values[Freq].value
                                                       << ";" << _values[Amplitude].value
                                                       << ";" << _state[Speaker];
    }
    if (device == "DiagnosticLed")
    {
        AssisiMsg::ColorStamped LEDcolor;
        LEDcolor.ParseFromString(data);
        _ledColor.setRgbF(LEDcolor.color().red(),
                          LEDcolor.color().green(),
                          LEDcolor.color().blue());
        _state[LED] = command == "On";
        if(settings->value("log_on").toBool()) _logFile << ";" << _ledColor.name().toStdString()
                                                       << ";" << _state[LED];
    }

    _logFile << endl;
}

void QCasuZMQ::connectionTimeout()
{
    _connected = false;
    _connectionTimer->stop();
}
