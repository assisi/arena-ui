#ifndef QCASUZMQ_H
#define QCASUZMQ_H

#define findKey(l, k) (std::find(l.begin(),l.end(), k) != l.end())

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
        Freq, Amp, // m_vibr_num = 2
        Peltier, Airflow, Speaker, VibePatt, Speaker_freq, Speaker_amp, VibePatt_period, VibePatt_freq, VibePatt_amp,  LED};

    const static std::vector<dataType> m_IR_ARRAY = {IR_F, IR_FL, IR_BL, IR_B, IR_BR, IR_FR};
    const static std::vector<dataType> m_TEMP_ARRAY = {Temp_F, Temp_L, Temp_B, Temp_R, Temp_Top, Temp_Pcb, Temp_Ring, Temp_Wax};
    const static std::vector<dataType> m_VIBE_ARRAY = {Freq, Amp};
    const static std::vector<dataType> m_SETPOINT_ARRAY = {Peltier, Airflow, Speaker, VibePatt};
    const static std::vector<dataType> m_SPEAKER_ARRAY = {Speaker_freq, Speaker_amp};
    const static std::vector<dataType> m_VIBEPATT_ARRAY = {VibePatt_period, VibePatt_freq, VibePatt_amp};

    const static std::vector<dataType> m_DATA_BUFFERS = {IR_F, IR_FL, IR_BL, IR_B, IR_BR, IR_FR,
                                                         Temp_F, Temp_L, Temp_B, Temp_R, Temp_Top, Temp_Pcb, Temp_Ring, Temp_Wax, Peltier};
    const static std::vector<dataType> m_DATA_SETPOINT = {Airflow, Speaker, VibePatt, Speaker_freq,
                                                         Speaker_amp, VibePatt_period, VibePatt_freq, VibePatt_amp,  LED};

    class zmqBuffer : public QObject, public QCPGraphDataContainer
        {
            Q_OBJECT
        private:
            QString m_legendName;
            QString m_casuName;
            dataType m_key;
        public:
            zmqBuffer(QString casuName, dataType key);
            void emitReplot();
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

    QSharedPointer<zmqData::zmqBuffer> getBuffer(zmqData::dataType key) const;
    double getLastValue(zmqData::dataType key) const;
    QList<QCPGraphData> getLastValuesList(zmqData::dataType key) const;
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

    QMap<zmqData::dataType, QSharedPointer<zmqData::zmqBuffer> > m_buffers;
    QMultiMap<zmqData::dataType, QCPGraphData> m_values;
    QMap<zmqData::dataType, double> m_lastDataTime;
    QMap<zmqData::dataType, bool> m_state;
    QColor m_ledColor;

    std::map<QString, std::ofstream> m_logFile;
    std::map<QString, QString> m_logName;
    std::map<QString, bool> m_logOpen;

    bool m_connected = false;

    void openLogFile(QString);
    void closeLogFile(QString);

    void addToBuffer(zmqData::dataType key, QCPGraphData data);
    void connectZMQ();

signals:
    void updated(zmqData::dataType key);
    void connectMsg(const QString &message);

private slots:
    void messageReceived(const QList<QByteArray> &message);
};

#endif // QCASUZMQ_H
