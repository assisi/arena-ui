#include "qcasutreeitem.h"
#define roundF2D(x) (round(x*100.0)/100.0)

using namespace zmqData;

QCasuTreeItem::QCasuTreeItem(QCasuZMQ *zmqObject) :
    m_zmqObject(zmqObject)
{
    setData(0,Qt::DisplayRole,QStringList(m_zmqObject->getName()));
    QTreeWidgetItem* tempWidget;

    tempWidget = new QNoSortTreeItem(QStringList("LED color"));
    tempWidget->setFlags(Qt::ItemIsEnabled);
    m_widgetMap.insert(LED, tempWidget);
    addChild(tempWidget);

    //zadavanje djece IR grani:
    {
        tempWidget = new QNoSortTreeItem(QStringList("IR - Proximity"));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("IR - F")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("IR - FL")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("IR - BL")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("IR - B")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("IR - BR")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("IR - FR")));
        for(int k = 0; k < m_IR_NUM; k++){
            m_widgetMap.insert(dCast(k), tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        addChild(tempWidget);
    }

    //zadavanje djece temp grani:
    {
        tempWidget = new QNoSortTreeItem(QStringList("Temperature"));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Temp - F")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Temp - L")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Temp - B")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Temp - R")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Temp - TOP")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Temp - PCB")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Temp - RING")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Temp - WAX")));
        for(int k = 0; k < m_TEMP_NUM; k++){
            m_widgetMap.insert(dCast(k + m_TEMP_START), tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        addChild(tempWidget);
    }
    //zadavanje djece vibr grani:
    {
        tempWidget = new QNoSortTreeItem(QStringList("Vibration"));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Frequency")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Amplitude")));
        for(int k = 0; k < 2; k++){
            m_widgetMap.insert(dCast(k + m_VIBR_START), tempWidget->child(k));
            tempWidget->setFlags(Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        addChild(tempWidget);
    }
    //zadavanje djece setpoint grani:
    {
        tempWidget = new QNoSortTreeItem(QStringList("Current setpoints"));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Peltier")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Airflow")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Speaker")));
        tempWidget->child(2)->addChild(new QNoSortTreeItem(QStringList("Frequency")));
        tempWidget->child(2)->addChild(new QNoSortTreeItem(QStringList("Amplitude")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("VibrationPattern")));
        tempWidget->child(3)->addChild(new QNoSortTreeItem(QStringList("Period")));
        tempWidget->child(3)->addChild(new QNoSortTreeItem(QStringList("Frequency")));
        tempWidget->child(3)->addChild(new QNoSortTreeItem(QStringList("Amplitude")));
        for(int k = 0; k < m_SETPOINT_NUM; k++){
            m_widgetMap.insert(dCast(k + m_SETPOINT_START), tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsEnabled);
        }
        for(int k = 0; k < m_SETPOINT_SPEAKER_NUM; k++){
            m_widgetMap.insert(dCast(k + m_SETPOINT_SPEAKER_START), tempWidget->child(2)->child(k));
            tempWidget->child(2)->child(k)->setFlags(Qt::ItemIsEnabled);
        }
        for(int k = 0; k < m_SETPOINT_VIBEPATT_NUM; k++){
            m_widgetMap.insert(dCast(k + m_SETPOINT_VIBEPATT_START), tempWidget->child(3)->child(k));
            tempWidget->child(3)->child(k)->setFlags(Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        addChild(tempWidget);
    }

    setFlags(Qt::ItemIsEnabled);

    m_zmqObjectConnection = connect (m_zmqObject,&QCasuZMQ::updated,[&](dataType key){
        if(m_widgetMap.isEmpty()) return;
        if(key == LED){
            if(m_zmqObject->getState(LED)) m_widgetMap[key]->setData(1, Qt::DisplayRole, QVariant(m_zmqObject->getLedColor().name()));
            else m_widgetMap[key]->setData(1, Qt::DisplayRole, QVariant());
            m_widgetMap[key]->setTextColor(1, m_zmqObject->getLedColor());
            return;
        }
        if(key == Freq){
            QString temp_freq;
            QString temp_amp;
            for(auto &item : m_zmqObject->getLastValuesList(Freq))
                temp_freq = QString::number((int)item.value)+ "\t" + temp_freq;
            for(auto &item : m_zmqObject->getLastValuesList(Amp))
                temp_amp = QString::number((int)item.value)+ "\t" + temp_amp;

            // padding last values to left justify last column
            temp_freq.chop(1);
            temp_amp.chop(1);
            int max_size = temp_freq.size() - temp_freq.lastIndexOf('\t');
            if(temp_amp.size() - temp_amp.lastIndexOf('\t') > max_size) max_size = temp_amp.size() - temp_amp.lastIndexOf('\t');
            temp_freq = temp_freq.leftJustified(temp_freq.lastIndexOf('\t') + max_size);
            temp_amp = temp_amp.leftJustified(temp_amp.lastIndexOf('\t') + max_size);

            m_widgetMap[Freq]->setData(1, Qt::DisplayRole, QVariant(temp_freq));
            m_widgetMap[Amp]->setData(1, Qt::DisplayRole, QVariant(temp_amp));
            return;
        }
        if(key == VibePatt){
            QString temp_period;
            QString temp_freq;
            QString temp_amp;
            for(auto &item : m_zmqObject->getLastValuesList(VibePatt_period))
                temp_period = QString::number((int)item.value)+ "\t" + temp_period;
            for(auto &item : m_zmqObject->getLastValuesList(VibePatt_freq))
                temp_freq = QString::number((int)item.value)+ "\t" + temp_freq;
            for(auto &item : m_zmqObject->getLastValuesList(VibePatt_amp))
                temp_amp = QString::number((int)item.value)+ "\t" + temp_amp;

            // padding last values to left justify last column
            temp_period.chop(1);
            temp_freq.chop(1);
            temp_amp.chop(1);
            int max_size = temp_period.size() - temp_period.lastIndexOf('\t');
            if(temp_freq.size() - temp_freq.lastIndexOf('\t') > max_size) temp_freq = temp_freq.size() - temp_amp.lastIndexOf('\t');
            if(temp_amp.size() - temp_amp.lastIndexOf('\t') > max_size) max_size = temp_amp.size() - temp_amp.lastIndexOf('\t');
            temp_period = temp_period.leftJustified(temp_period.lastIndexOf('\t') + max_size);
            temp_freq = temp_freq.leftJustified(temp_freq.lastIndexOf('\t') + max_size);
            temp_amp = temp_amp.leftJustified(temp_amp.lastIndexOf('\t') + max_size);

            m_widgetMap[VibePatt_period]->setData(1, Qt::DisplayRole, QVariant(temp_period));
            m_widgetMap[VibePatt_freq]->setData(1, Qt::DisplayRole, QVariant(temp_freq));
            m_widgetMap[VibePatt_amp]->setData(1, Qt::DisplayRole, QVariant(temp_amp));

            m_widgetMap[VibePatt_period]->setTextColor(1, m_zmqObject->getState(VibePatt_period)? Qt::darkGreen : Qt::red);
            m_widgetMap[VibePatt_freq]->setTextColor(1, m_zmqObject->getState(VibePatt_freq)? Qt::darkGreen : Qt::red);
            m_widgetMap[VibePatt_amp]->setTextColor(1, m_zmqObject->getState(VibePatt_amp)? Qt::darkGreen : Qt::red);
            return;
        }
        m_widgetMap[key]->setData(1, Qt::DisplayRole, QVariant(m_zmqObject->getLastValue(key)));
        if(key >= m_SETPOINT_START){
            m_widgetMap[key]->setTextColor(1, m_zmqObject->getState(key)? Qt::darkGreen : Qt::red);
        }
    });
}

QCasuTreeItem::~QCasuTreeItem()
{
    disconnect(m_zmqObjectConnection);
}
