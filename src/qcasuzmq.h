#ifndef QCASUZMQ_H
#define QCASUZMQ_H

#define dCast static_cast<zmqData::dataType>

#include <QObject>
#include <QTime>

#include <fstream>

#include <nzmqt/nzmqt.hpp>
#include "QCustomPlot/qcustomplot.h"

#include "dev_msgs.pb.h"
#include "globalHeader.h"

namespace zmqData {
    enum dataType {IR_F, IR_FL, IR_BL, IR_B, IR_BR, IR_FR, // m_IR_num = 6
        Temp_F, Temp_L, Temp_B, Temp_R, Temp_Top, Temp_Pcb, Temp_Ring, Temp_Wax, // m_Temp_num = 8
        Frequency, Amplitude, StdDev,
        Peltier, Airflow, Speaker, LED};

    const static int m_IR_NUM = 6;
    const static int m_temp_NUM = 8;
    const static int m_dataType_NUM = 21;

    class zmqBuffer : public QObject, public QCPDataMap
        {
            Q_OBJECT
        private:
            QString m_legendName;
            QString m_casuName;
            dataType m_key;
        public:
            zmqBuffer(QString casuName, dataType key);
            void insert(const double &key, const QCPData &value);
            void erase(QMap::iterator it);
            QString getLegendName() const;
            QString getCasuName() const;
            dataType getDataType() const;
            double getLastTime() const;
        signals:
            void updatePlot();
        };
}

class QCasuZMQ : public QObject
{
    Q_OBJECT
    friend class QCasuSceneItem;
public:
    explicit QCasuZMQ(QObject *parent = 0, QString casuName = QString());

    zmqData::zmqBuffer* getBuffer(zmqData::dataType key) const;
    double getValue(zmqData::dataType key) const;
    QColor getLedColor() const;
    bool getState(zmqData::dataType key) const;
    int getAvgSamplingTime() const;
    QString getName() const;
    QStringList getAddresses() const;

    void setAddresses(QString sub, QString pub, QString msg);
    void setAddresses(QStringList addresses);

    bool sendSetpoint(QList<QByteArray> message);

    bool isConnected() const;


private:
    nzmqt::ZMQContext* m_context;
    nzmqt::ZMQSocket* m_pubSock;
    nzmqt::ZMQSocket* m_subSock;

    QString m_subAddr;
    QString m_pubAddr;
    QString m_msgAddr;
    QString m_name;

    QTimer* m_connectionTimer;

    QMap<zmqData::dataType, zmqData::zmqBuffer*> m_buffers;
    QMap<zmqData::dataType, QCPData> m_values;
    QMap<zmqData::dataType, double> m_lastDataTime;
    QMap<zmqData::dataType, bool> m_state;
    QColor m_ledColor;

    std::ofstream m_logFile;
    QString m_logName;
    bool m_logOpen;

    bool m_connected = false;

    void openLogFile();
    void closeLogFile();

    void addToBuffer(zmqData::dataType key, QCPData data);
    void connectZMQ();

signals:
    void updated(zmqData::dataType key);
    void connectMsg(const QString &message);

private slots:
    void messageReceived(const QList<QByteArray> &message);
};

#endif // QCASUZMQ_H