#include "qcasuzmq.h"

using namespace zmqData;

QCasuZMQ::QCasuZMQ(QObject *parent, QString casuName) :
    QObject(parent),
    m_name(casuName)
{
    for(uint k = 0; k < m_DATA_BUFFERS.size(); k++){
        m_buffers.insert(m_DATA_BUFFERS[k], QSharedPointer<zmqBuffer>::create(m_name, m_DATA_BUFFERS[k]));
    }
    for(uint k = 0; k < m_DATA_SETPOINT.size(); k++){
        m_state.insert(m_DATA_SETPOINT[k], false);
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

QSharedPointer <zmqData::zmqBuffer> QCasuZMQ::getBuffer(dataType key) const
{
    return m_buffers[key];
}

double QCasuZMQ::getLastValue(dataType key) const
{
    if (findKey(m_DATA_BUFFERS, key)){
        bool isValid;
        QCPRange bufferRange = m_buffers[key]->valueRange(isValid);
        if (isValid) return bufferRange.upper;
        else return 0;
    }
    return m_values.value(key).value;
}

QList<QCPGraphData> QCasuZMQ::getLastValuesList(dataType key) const
{
    return m_values.values(key);
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
        if(findKey(m_DATA_BUFFERS, key)){
            result += m_buffers[key]->getLastTime() - oldTime;
        } else {
            result += m_values.value(key).key - oldTime;
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

void QCasuZMQ::openLogFile(QString device)
{
    if(!QDir(g_settings->value("logSubFolder").toString() + m_name).exists())QDir().mkdir(g_settings->value("logSubFolder").toString() + m_name);
    if(!QDir(g_settings->value("logSubFolder").toString() + m_name + "/" + device).exists())QDir().mkdir(g_settings->value("logSubFolder").toString() + m_name + "/" + device);
    m_logName[device] = g_settings->value("logSubFolder").toString() + m_name + "/" + device + "/" + QDateTime::currentDateTime().toString(g_date_time_format) + ".log";
    m_logFile[device].open(m_logName[device].toStdString().c_str(), std::ofstream::out | std::ofstream::app);
    m_logOpen[device] = true;
}

void QCasuZMQ::closeLogFile(QString device)
{
    m_logFile[device].close();
    m_logOpen[device] = false;
}

void QCasuZMQ::addToBuffer(dataType key, QCPGraphData data)
{
    bool t_plot_to_update = false;
    if(m_buffers[key]->isEmpty() || (data.key -  m_buffers[key]->getLastTime())*1000 >g_settings->value("trendSampleTime_ms").toDouble()){
        m_buffers[key]->add(data);
        t_plot_to_update = true;
    }

    // check if trendTimeSpan would empty whole buffer
    // make certain that at least one data point stays inside

    double tempTime1 = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0 - QTime(0,0,0).secsTo(g_settings->value("trendTimeSpan").toTime());
    double tempTime2 = m_buffers[key]->getLastTime() - QTime(0,0,0).secsTo(g_settings->value("trendSampleTime_ms").toTime());
    m_buffers[key]->removeBefore(tempTime1 < tempTime2 ? tempTime1 : tempTime2);

    if (t_plot_to_update){
        m_buffers[key]->emitReplot();
    }
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

    m_connectionTimer->start(2000);

    QString device(message.at(1));
    QString command(message.at(2));
    std::string data(message.at(3).constData(), message.at(3).size());

    QCPGraphData newData;
    newData.key = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0;

    if(g_settings->value("log_on").toBool() && !m_logOpen[device]) openLogFile(device);
    if(!g_settings->value("log_on").toBool() && m_logOpen[device]) closeLogFile(device);
    m_logFile[device] << QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0, 'f', 3).toStdString() ;

    if (device == "IR"){
        AssisiMsg::RangeArray ranges;
        ranges.ParseFromString(data);
        m_lastDataTime[IR_F] = m_buffers[IR_F]->getLastTime();
        for (uint k = 0; k < ranges.raw_value_size(); k++){
            if( k == m_IR_ARRAY.size()) break;
            newData.value = ranges.raw_value(k);
            this->addToBuffer(m_IR_ARRAY[k], newData);
            // ---- UPDATE ---- //
            emit updated(m_IR_ARRAY[k]);
            // ---- LOG ---- //
            if(g_settings->value("log_on").toBool()){
                m_logFile[device]<< " ; " << newData.value;
            }
        }
    }
    if (device == "Temp"){
        AssisiMsg::TemperatureArray temperatures;
        temperatures.ParseFromString(data);
        m_lastDataTime[Temp_F] = m_buffers[Temp_F]->getLastTime();
        for (uint k = 0; k < temperatures.temp_size(); k++){
            if( k == m_TEMP_ARRAY.size()) break;
            newData.value = temperatures.temp(k);
            this->addToBuffer(m_TEMP_ARRAY[k], newData);
            // ---- UPDATE ---- //
            emit updated(m_TEMP_ARRAY[k]);
            // ---- LOG ---- //
            if(g_settings->value("log_on").toBool()){
                m_logFile[device]<< " ; " << newData.value;
            }
        }
    }
    if (device == "Fft"){
        AssisiMsg::VibrationReadingArray vibrationsArray;
        vibrationsArray.ParseFromString(data);
        AssisiMsg::VibrationReading vibrations = vibrationsArray.reading(0);
        m_lastDataTime[Freq] = m_values.value(Freq).key;
        m_values.remove(Freq);
        m_values.remove(Amp);
        int k = 0;
        for(; k < vibrations.freq_size(); k++){
            if(k == 2) break; //stop at second reading
            newData.value = vibrations.freq(k);
            m_values.insert(Freq, newData);
            newData.value = vibrations.amplitude(k);
            m_values.insert(Amp, newData);
            // ---- LOG pt 1 ---- //
            if(g_settings->value("log_on").toBool()){
                m_logFile[device] << " ; " << m_values.value(Freq).value
                                  << " ; " << m_values.value(Amp).value;
            }
        }
        for(; k < 2; k++){// Fill rest of values with zero
            newData.value = 0.0;
            m_values.insert(Freq, newData);
            m_values.insert(Amp, newData);
            // ---- LOG pt 2 ---- //
            if(g_settings->value("log_on").toBool()){
                m_logFile[device] << " ; " << m_values.value(Freq).value
                                  << " ; " << m_values.value(Amp).value;
            }
        }
        // ---- UPDATE ---- //
        emit updated(Freq);
    }
    if (device == "Peltier"){
        AssisiMsg::Temperature peltier;
        peltier.ParseFromString(data);
        newData.value = peltier.temp();
        m_lastDataTime[Peltier] = m_values.value(Peltier).key;
        this->addToBuffer(Peltier, newData);
        m_state[Peltier] = command == "On";
        // ---- LOG ---- //
        if(g_settings->value("log_on").toBool()){
            m_logFile[device] << " ; " << m_values.value(Peltier).value
                              << " ; " << m_state[Peltier];
        }
        // ---- UPDATE ---- //
        emit updated(Peltier);
     }
    if (device == "Airflow"){
        AssisiMsg::Airflow airflow;
        airflow.ParseFromString(data);
        newData.value = airflow.intensity();
        m_lastDataTime[Airflow] = m_values.value(Airflow).key;
        m_values.replace(Airflow, newData);
        m_state[Airflow] = command == "On";
        // ---- LOG ---- //
        if(g_settings->value("log_on").toBool()){
            m_logFile[device] << " ; " << m_values.value(Airflow).value
                              << " ; " << m_state[Airflow];
        }
        // ---- UPDATE ---- //
        emit updated(Airflow);
     }
    if (device == "Speaker"){
        AssisiMsg::VibrationSetpoint airflow;
        airflow.ParseFromString(data);
        newData.value = airflow.freq();
        m_lastDataTime[Speaker_freq] = m_values.value(Speaker_freq).key;
        m_values.replace(Speaker_freq, newData);
        newData.value = airflow.amplitude();
        m_values.replace(Speaker_amp, newData);
        m_state[Speaker] = command == "On";
        m_state[Speaker_freq] = command == "On";
        m_state[Speaker_amp] = command == "On";
        // ---- LOG ---- //
        if(g_settings->value("log_on").toBool()){
            m_logFile[device] << " ; " << m_values.value(Speaker_freq).value
                              << " ; " << m_values.value(Speaker_amp).value
                              << " ; " << m_state[Speaker];
        }
        // ---- UPDATE ---- //
        emit updated(Speaker_freq);
        emit updated(Speaker_amp);
    }
    if (device == "DiagnosticLed")
    {
        AssisiMsg::ColorStamped LEDcolor;
        LEDcolor.ParseFromString(data);
        m_ledColor.setRgbF(LEDcolor.color().red(),
                          LEDcolor.color().green(),
                          LEDcolor.color().blue());
        m_state[LED] = command == "On";
        // ---- LOG ---- //
        if(g_settings->value("log_on").toBool()){
            m_logFile[device]<< " ; " << m_ledColor.name().toStdString()
                      << " ; " << m_state[LED];
        }
        // ---- UPDATE ---- //
        emit updated(LED);
    }
    if (device == "VibrationPattern"){
        AssisiMsg::VibrationPattern vibrationPattern;
        vibrationPattern.ParseFromString(data);
        m_lastDataTime[VibePatt_freq] = m_values.value(VibePatt_freq).key;
        m_values.remove(VibePatt_period);
        m_values.remove(VibePatt_freq);
        m_values.remove(VibePatt_amp);
        m_state[VibePatt] = command == "On";
        m_state[VibePatt_period] = command == "On";
        m_state[VibePatt_freq] = command == "On";
        m_state[VibePatt_amp] = command == "On";
        int k = 0;
        for(; k < vibrationPattern.vibe_freqs_size(); k++){
            newData.value = vibrationPattern.vibe_periods(k);
            m_values.insert(VibePatt_period, newData);
            newData.value = vibrationPattern.vibe_freqs(k);
            m_values.insert(VibePatt_freq, newData);
            newData.value = vibrationPattern.vibe_amps(k);
            m_values.insert(VibePatt_amp, newData);
            if(g_settings->value("log_on").toBool()){
                m_logFile[device] << " ; " << m_values.value(VibePatt_period).value
                                  << " ; " << m_values.value(VibePatt_freq).value
                                  << " ; " << m_values.value(VibePatt_amp).value
                                  << " ; " << m_state[VibePatt];
            }
        }
        // ---- UPDATE ---- //
        emit updated(VibePatt);
    }

    m_logFile[device] << std::endl;
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

void zmqBuffer::emitReplot()
{
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
    if (this->isEmpty())
        return 0;
    else
        return std::prev(this->constEnd())->key;
}
