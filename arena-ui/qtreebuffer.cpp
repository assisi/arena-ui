#include "qtreebuffer.h"

QTreeBuffer::QTreeBuffer(const QStringList & strings, QString lName, QWidget *parent) :
    QObject(parent),
    QTreeWidgetItem(strings),
    buffer(new QCPDataMap),
    lastDataTime(QTime(0,0,0)),
    legendName(lName){}

QTreeBuffer::~QTreeBuffer(){
    delete buffer;
}

void QTreeBuffer::addToBuffer(QTime time, double value)
{
    if(lastDataTime.msecsTo(time) < trendSampleTime_ms) return;
    else lastDataTime = time;

    QCPData newData;
    newData.key = (double) QTime(0,0,0).msecsTo(time) /1000;
    newData.value = value;

    buffer->insert(newData.key, newData);
    while(newData.key - buffer->begin()->key > QTime(0,0,0).secsTo(trendTimeSpan)) buffer->erase(buffer->begin()); //Delete data older than $timeSpan

    emit updatePlot(newData.key, value);
}
