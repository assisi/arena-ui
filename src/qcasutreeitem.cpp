#include "qcasutreeitem.h"

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
        for(int k = 0; k < 2; k++){
            m_widgetMap.insert(dCast(k + m_SETPOINT_START), tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsEnabled);
        }
        for(int k = 0; k < 2; k++){
            m_widgetMap.insert(dCast(k + m_SETPOINT_START + 3), tempWidget->child(2)->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsEnabled);
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
        if(key == Freq1){
            m_widgetMap[Freq1]->setData(1, Qt::DisplayRole, QVariant(m_zmqObject->getValue(Freq1)));
            m_widgetMap[Amp1]->setData(1, Qt::DisplayRole, QVariant(m_zmqObject->getValue(Amp1)));
            return;
        }
        if(key == Freq2){
            QString temp_freq = QString::number(m_zmqObject->getValue(Freq1)) + " " + QString::number(m_zmqObject->getValue(Amp1));
            QString temp_amp = QString::number(m_zmqObject->getValue(Amp2)) + " " + QString::number(m_zmqObject->getValue(Amp2));
            m_widgetMap[Freq1]->setData(1, Qt::DisplayRole, QVariant(temp_freq));
            m_widgetMap[Amp1]->setData(1, Qt::DisplayRole, QVariant(temp_amp));
            return;
        }
        m_widgetMap[key]->setData(1, Qt::DisplayRole, QVariant(m_zmqObject->getValue(key)));
        if(key >= m_SETPOINT_START){
            m_widgetMap[key]->setTextColor(1, m_zmqObject->getState(key)? Qt::green : Qt::red);
        }
    });
}

QCasuTreeItem::~QCasuTreeItem()
{
    disconnect(m_zmqObjectConnection);
}
