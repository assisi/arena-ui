#ifndef QCASUZMQ_H
#define QCASUZMQ_H

#include <QObject>

#include "QCustomPlot/qcustomplot.h"

class QCasuZMQ : public QObject
{
    Q_OBJECT
public:
    enum dataType {IR_F, IR_FL, IR_BL, IR_B, IR_BR, IR_FR,
                  Temp_F, Temp_L, Temp_B, Temp_R, Temp_Top, Temp_Pcb, Temp_Wax}
                  /*Freq, Ampl, StdDev,
                  Peltier, Airflow, Speaker}*/

    explicit QCasuZMQ(QObject *parent = 0);
    QCPDataMap* getBuffer(dataType key);
    QCPDataMap* getValue(dataType key);


private:
    QMap<dataType, QCPDataMap*> _buffers;
    static int _IR_num = 6;
    static int _Temp_num = 7;

    void insertValue(dataType key);

    void addToBuffer(dataType key, QCPData value);

signals:


public slots:

};

#endif // QCASUZMQ_H
