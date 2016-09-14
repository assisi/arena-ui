#include "qcasuzmq.h"

using namespace zmqData;

QCasuZMQ::QCasuZMQ(QObject *parent, QString casuName) :
    QObject(parent),
    m_name(casuName)
{
    for(int k = 0; k < m_IR_NUM + m_temp_NUM; k++){
        m_buffers.insert(dCast(k), new zmqBuffer(m_name, dCast(k)));
    }
    for(int k = m_IR_NUM + m_temp_NUM; k < m_dataType_NUM; k++){
        m_state.insert(dCast(k), false);
    }

    m_connectionTimer = new QTimer(this);

    m_context = nzmqt::createDefaultContext(this);
    m_context->start();
    m_subSock = m_context->createSocket(nzmqt::ZMQSocket::TYP_SUB, this);
    m_pubSock = m_context->createSocket(nzmqt::ZMQSocket::TYP_PUB, this);

    connect(m_subSock, &nzmqt::ZMQSocket::messageReceived, this, &QCasuZMQ::messageReceived);
    connect(m_connectionTimer, &QTimer::timeout,[&](){
        m_connected = false;
        m_connectionTimer->stop();
         emit connectMsg("[ZMQ][" + m_name + "][ERR] Connection timeout!");
    });
}

zmqBuffer *QCasuZMQ::getBuffer(dataType key) const
{
    if (key < m_IR_NUM + m_temp_NUM) return m_buffers[key];
    return 0;
}

double QCasuZMQ::getValue(dataType key) const
{
    if (key < m_IR_NUM + m_temp_NUM){
        if (m_buffers[key]->isEmpty()) return 0;
        else return m_buffers[key]->last().value;
    }
    return m_values[key].value;
}

QColor QCasuZMQ::getLedColor() const
{
    return m_state[LED] ? m_ledColor : Qt::gray;
}

bool QCasuZMQ::getState(dataType key) const
{
    return m_state[key];
}

int QCasuZMQ::getAvgSamplingTime() const
{
    double result = 0;
    double itemNum = 0;

    for(auto& oldTime : m_lastDataTime){
        dataType key = m_lastDataTime.key(oldTime, LED);
        if(key == LED) continue;
        if(key >= m_IR_NUM + m_temp_NUM){
            result += m_values[key].key - oldTime;
        } else {
            result += m_buffers[key]->getLastTime() - oldTime;
        }
        itemNum++;
    }
    return itemNum && m_connected ? result*1000/itemNum : 0;
}

QString QCasuZMQ::getName() const
{
    return m_name;
}

QStringList QCasuZMQ::getAddresses() const
{
    QStringList out;
    out.append(m_subAddr);
    out.append(m_pubAddr);
    out.append(m_msgAddr);
    return out;
}

void QCasuZMQ::setAddresses(QString sub, QString pub, QString msg)
{
    if(m_connected){
        m_subSock->unsubscribeFrom("casu");
        m_subSock->disconnectFrom(m_subAddr);
        m_pubSock->disconnectFrom(m_pubAddr);
        emit connectMsg("[ZMQ][" + m_name + "] Disconnected");
    }
    m_subAddr = sub;
    m_pubAddr = pub;
    m_msgAddr = msg;

    this->connectZMQ();
}

void QCasuZMQ::setAddresses(QStringList addresses)
{
    setAddresses(addresses.at(0), addresses.at(1), addresses.at(2));
}

bool QCasuZMQ::sendSetpoint(QList<QByteArray> message)
{
    if(!m_connected) return false;
    message.push_front(QString(m_name).toLocal8Bit());
    return m_pubSock->sendMessage(message);
}

bool QCasuZMQ::isConnected() const
{
    return m_connected;
}

void QCasuZMQ::openLogFile()
{
    m_logName = g_settings->value("logSubFolder").toString() + QDateTime::currentDateTime().toString(g_date_time_format) + m_name + ".log";
    m_logFile.open(m_logName.toStdString().c_str(), std::ofstream::out | std::ofstream::app);
    m_logOpen = true;
}

void QCasuZMQ::closeLogFile()
{
    m_logFile.close();
    m_logOpen = false;
}

void QCasuZMQ::addToBuffer(dataType key, QCPData data)
{
    m_buffers[key]->insert(data.key, data);
    while(data.key - m_buffers[key]->firstKey() > QTime(0,0,0).secsTo(g_settings->value("trendTimeSpan").toTime())){
        m_buffers[key]->erase(m_buffers[key]->begin()); //Delete data older than $timeSpan
    }

    emit updated(key);
}

void QCasuZMQ::connectZMQ()
{
    try{
        m_pubSock->connectTo(m_pubAddr);
        // Subscribe to everything!
        m_subSock->subscribeTo("casu");
        m_subSock->connectTo(m_subAddr);
        m_connectionTimer->start(1000);
    }
    catch(zmq::error_t &e){
        emit connectMsg("[ZMQ][" + m_name + "][ERR] Failed to connect: " + QString(e.what()));
        m_connected = false;
    }
}

void QCasuZMQ::messageReceived(const QList<QByteArray> &message)
{
    QString name(message.at(0));
    if(name != m_name) return;
    if(!m_connected) {
        m_connected = true;
         emit connectMsg("[ZMQ][" + m_name + "] Connected");
    }
    if(g_settings->value("log_on").toBool() && !m_logOpen) openLogFile();
    if(!g_settings->value("log_on").toBool() && m_logOpen) closeLogFile();

    m_connectionTimer->start(2000);

    QString device(message.at(1));
    QString command(message.at(2));
    std::string data(message.at(3).constData(), message.at(3).size());

    QCPData newData;
    newData.key = (double) QTime(0,0,0).msecsTo(QTime::currentTime())/1000;

    m_logFile << device.toStdString() << ";" << (float) QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000 ;

    if (device == "IR"){
        AssisiMsg::RangeArray ranges;
        ranges.ParseFromString(data);
        m_lastDataTime[dCast(0)] = m_buffers[dCast(0)]->getLastTime();
        for (int k = 0; k < ranges.raw_value_size(); k++){
            if( k == m_IR_NUM) break;
            newData.value = ranges.raw_value(k);
            this->addToBuffer(dCast(k), newData);
            emit updated(dCast(k));
            if(g_settings->value("log_on").toBool()){
                m_logFile << ";" << newData.value;
            }
        }
    }
    if (device == "Temp"){
        AssisiMsg::TemperatureArray temperatures;
        temperatures.ParseFromString(data);
        m_lastDataTime[dCast(m_IR_NUM)] = m_buffers[dCast(m_IR_NUM)]->getLastTime();
        for (int k = 0; k < temperatures.temp_size(); k++){
            if( k == m_temp_NUM) break;
            newData.value = temperatures.temp(k);
            this->addToBuffer(dCast(k+m_IR_NUM), newData);
            emit updated(dCast(k+m_IR_NUM));
            if(g_settings->value("log_on").toBool()){
                m_logFile << ";" << newData.value;
            }
        }
    }

// TODO: Implement Vibration measurements

    if (device == "Peltier"){
        AssisiMsg::Temperature pelt;
        pelt.ParseFromString(data);
        newData.value = pelt.temp();
        m_lastDataTime[Peltier] = m_values[Peltier].key;
        m_values[Peltier] = newData;
        m_state[Peltier] = command == "On";
        emit updated(Peltier);
        if(g_settings->value("log_on").toBool()){
            m_logFile << ";" << m_values[Peltier].value
                      << ";" << m_state[Peltier];
        }
     }
    if (device == "Airflow"){
        AssisiMsg::Airflow air;
        air.ParseFromString(data);
        m_lastDataTime[Airflow] = m_values[Airflow].key;
        newData.value = air.intensity();
        m_values[Airflow] = newData;
        m_state[Airflow] = command == "On";
        emit updated(Airflow);
        if(g_settings->value("log_on").toBool()){
            m_logFile << ";" << m_values[Airflow].value
                      << ";" << m_state[Airflow];
        }

     }
    if (device == "Speaker"){
        AssisiMsg::VibrationSetpoint vibr;
        vibr.ParseFromString(data);
         m_lastDataTime[Frequency] = m_values[Frequency].key;
        newData.value = vibr.freq();
        m_values[Frequency] = newData;
        newData.value = vibr.amplitude();
        m_values[Amplitude] = newData;
        m_state[Speaker] = command == "On";
        m_state[Frequency] = command == "On";
        m_state[Amplitude] = command == "On";
        emit updated(Frequency);
        emit updated(Amplitude);
        if(g_settings->value("log_on").toBool()){
            m_logFile << ";" << m_values[Frequency].value
                      << ";" << m_values[Amplitude].value
                      << ";" << m_state[Speaker];
        }
    }
    if (device == "DiagnosticLed")
    {
        AssisiMsg::ColorStamped LEDcolor;
        LEDcolor.ParseFromString(data);
        m_ledColor.setRgbF(LEDcolor.color().red(),
                          LEDcolor.color().green(),
                          LEDcolor.color().blue());
        m_state[LED] = command == "On";
        emit updated(LED);
        if(g_settings->value("log_on").toBool()){
            m_logFile << ";" << m_ledColor.name().toStdString()
                      << ";" << m_state[LED];
        }
    }

    m_logFile << std::endl;
}

// ----------------------------------------------------------------

zmqBuffer::zmqBuffer(QString casuName, dataType key) :
    m_legendName(casuName),
    m_casuName(casuName),
    m_key(key)
{
    switch(key){
        case IR_F  : m_legendName += ": IR - F";  break;
        case IR_FL : m_legendName += ": IR - FL"; break;
        case IR_FR : m_legendName += ": IR - FR"; break;
        case IR_B  : m_legendName += ": IR - B";  break;
        case IR_BR : m_legendName += ": IR - BR"; break;
        case IR_BL : m_legendName += ": IR - BL"; break;

        case Temp_F : m_legendName += ": Temp - F"; break;
        case Temp_R : m_legendName += ": Temp - R"; break;
        case Temp_B : m_legendName += ": Temp - B"; break;
        case Temp_L : m_legendName += ": Temp - L"; break;
        case Temp_Top : m_legendName += ": Temp - Top"; break;
        case Temp_Pcb : m_legendName += ": Temp - Pcb"; break;
        case Temp_Ring : m_legendName += ": Temp - Ring"; break;
        case Temp_Wax : m_legendName += ": Temp - Wax"; break;
        default: break;
    }
}

void zmqBuffer::insert(const double &key, const QCPData &value)
{
    this->QMap::insert(key, value);
    emit updatePlot();
}

void zmqBuffer::erase(QMap::iterator it)
{
    this->QMap::erase(it);
    emit updatePlot();
}

QString zmqBuffer::getLegendName() const
{
    return m_legendName;
}

QString zmqBuffer::getCasuName() const
{
    return m_casuName;
}

dataType zmqBuffer::getDataType() const
{
    return m_key;
}

double zmqBuffer::getLastTime() const
{
    if (this->isEmpty()) return 0;
    return last().key;
}
