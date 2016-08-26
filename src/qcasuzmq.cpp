#include "qcasuzmq.h"

QCasuZMQ::QCasuZMQ(QObject *parent) :
    QObject(parent)
{
    for(int k = 0; k < _IR_num + _Temp_num; k++) _buffers.insert(static_cast<dataType>(k), new QCPData());

}

QCPDataMap *QCasuZMQ::getBuffer(QCasuZMQ::dataType key)
{
    return _buffers[key];
}

QCPDataMap *QCasuZMQ::getValue(QCasuZMQ::dataType key)
{
    return _buffers[key]
}

void QCasuZMQ::addToBuffer(dataType key, QCPData value)
{
    if(lastDataTime.msecsTo(key) < settings->value("trendSampleTime_ms").toInt()) return;
    else lastDataTime = key;

    QCPData newData;
    newData.key = (double) QTime(0,0,0).msecsTo(key)/1000;
    newData.value = Value;

    buffer->insert(newData.key, newData);
    while(newData.key - buffer->begin()->key > QTime(0,0,0).secsTo(settings->value("trendTimeSpan").toTime())) buffer->erase(buffer->begin()); //Delete data older than $timeSpan

    emit update();
}
