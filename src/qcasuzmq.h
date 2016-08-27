#ifndef QCASUZMQ_H
#define QCASUZMQ_H

#include <QObject>
#include <QTime>

#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/functional/hash.hpp>

#include <nzmqt/nzmqt.hpp>
#include "QCustomPlot/qcustomplot.h"

#include "dev_msgs.pb.h"
#include "globalHeader.h"

using namespace nzmqt;
using namespace boost;
using namespace std;

class QCasuZMQ : public QObject
{
    Q_OBJECT
public:
    enum dataType {IR_F, IR_FL, IR_BL, IR_B, IR_BR, IR_FR, // _IR_num = 6
            Temp_F, Temp_L, Temp_B, Temp_R, Temp_Top, Temp_Pcb, Temp_Wax, // _Temp_num = 7
            Freq, Amplitude, StdDev,
            Peltier, Airflow, Speaker, LED};

    explicit QCasuZMQ(QObject *parent = 0, QString casuName = QString());
    QCPDataMap* getBuffer(dataType key);
    double getValue(dataType key);
    QColor getLedColor();
    bool getState(dataType key);
    double getAvgSamplingTime();

    void setAddress(QString sub, QString pub, QString msg);
    bool sendSetpoint(QList<QByteArray> message);

    bool isConnected();


private:
    nzmqt::ZMQContext* _context;
    nzmqt::ZMQSocket* _pubSock;
    nzmqt::ZMQSocket* _subSock;

    QString _subAddr;
    QString _pubAddr;
    QString _msgAddr;
    QString _name;

    QTimer* _connectionTimer;

    QMap<dataType, QCPDataMap*> _buffers;
    QMap<dataType, QCPData> _values;
    QMap<dataType, double> _lastDataTime;
    QMap<dataType, bool> _state;
    QColor _ledColor;

    const static int _IR_num = 6;
    const static int _Temp_num = 7;
    const static int _dataType_num = 19;

    ofstream _logFile;
    QString _logName;
    bool _logOpen;

    bool _connected = false;

    void openLogFile();
    void closeLogFile();

    void addToBuffer(dataType key, QCPData data);
    void connectZMQ();

signals:
    void updated(dataType key);

private slots:
    void messageReceived(const QList<QByteArray>& message);
    void connectionTimeout();
};

#endif // QCASUZMQ_H
