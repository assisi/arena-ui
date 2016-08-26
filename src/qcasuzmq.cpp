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

QCPData *QCasuZMQ::getValue(QCasuZMQ::dataType key)
{
    return _buffers[key]->last().value;
}

void QCasuZMQ::addToBuffer(dataType key, QCPData data)
{
    _buffers[key]->insert(data.key, data);
    while(data.key - _buffers->begin()->key > QTime(0,0,0).secsTo(settings->value("trendTimeSpan").toTime())) _buffers->erase(_buffers->begin()); //Delete data older than $timeSpan

    emit updated(key);
}
