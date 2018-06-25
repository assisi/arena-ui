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
        for(uint k = 0; k < m_IR_ARRAY.size(); k++){
            m_widgetMap.insert(m_IR_ARRAY[k], tempWidget->child(k));
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
        for(uint k = 0; k < m_TEMP_ARRAY.size(); k++){
            m_widgetMap.insert(m_TEMP_ARRAY[k], tempWidget->child(k));
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
        for(uint k = 0; k < m_VIBE_ARRAY.size(); k++){
            m_widgetMap.insert(m_VIBE_ARRAY[k], tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsEnabled);
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
        for(uint k = 0; k < m_SETPOINT_ARRAY.size(); k++){
            m_widgetMap.insert(m_SETPOINT_ARRAY[k], tempWidget->child(k));
            if (k == 0) // CHECK IF CHILD IS PELTIER TO MAKE IT SELECTABLE
                tempWidget->child(0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            else
                tempWidget->child(k)->setFlags(Qt::ItemIsEnabled);
        }
        for(uint k = 0; k < m_SPEAKER_ARRAY.size(); k++){
            m_widgetMap.insert(m_SPEAKER_ARRAY[k], tempWidget->child(2)->child(k));
            tempWidget->child(2)->child(k)->setFlags(Qt::ItemIsEnabled);
        }
        for(uint k = 0; k < m_VIBEPATT_ARRAY.size(); k++){
            m_widgetMap.insert(m_VIBEPATT_ARRAY[k], tempWidget->child(3)->child(k));
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
        auto makeString = [&](dataType key){
                QString temp;
                int lastSize;
                for(auto &item : m_zmqObject->getLastValuesList(key)){
                    temp = QString::number((int)item.value)+ "\t" + temp;
                    lastSize = QString::number((int)item.value).size();
                }
                temp.chop(1);
                return std::pair<QString, int>(temp, lastSize);
            };
        if(key == Freq){
            std::vector<std::pair<QString,int>> dataStrings = {makeString(Freq), makeString(Amp)};

            // padding last values to left justify last column
            int max_size = dataStrings[0].second > dataStrings[1].second ? dataStrings[0].second : dataStrings[1].second;
            for(auto& item : dataStrings)
                item.first = item.first.leftJustified(item.first.lastIndexOf('\t') + max_size);

            m_widgetMap[Freq]->setData(1, Qt::DisplayRole, QVariant(dataStrings[0].first));
            m_widgetMap[Amp]->setData(1, Qt::DisplayRole, QVariant(dataStrings[1].first));
            return;
        }
        if(key == VibePatt){
            std::vector<std::pair<QString,int>> dataStrings = {makeString(VibePatt_period), makeString(VibePatt_freq), makeString(VibePatt_amp)};

            // padding last values to left justify last column
            int max_size = dataStrings[0].second > dataStrings[1].second ? dataStrings[0].second : dataStrings[1].second;
                max_size = dataStrings[2].second > max_size              ? dataStrings[2].second : max_size;
            for(auto& item : dataStrings)
                item.first = item.first.leftJustified(item.first.lastIndexOf('\t') + max_size);

            m_widgetMap[VibePatt_period]->setData(1, Qt::DisplayRole, QVariant(dataStrings[0].first));
            m_widgetMap[VibePatt_freq]->setData(1, Qt::DisplayRole, QVariant(dataStrings[1].first));
            m_widgetMap[VibePatt_amp]->setData(1, Qt::DisplayRole, QVariant(dataStrings[2].first));


            m_widgetMap[VibePatt_period]->setTextColor(1, m_zmqObject->getState(VibePatt_period)? Qt::darkGreen : Qt::red);
            m_widgetMap[VibePatt_freq]->setTextColor(1, m_zmqObject->getState(VibePatt_freq)? Qt::darkGreen : Qt::red);
            m_widgetMap[VibePatt_amp]->setTextColor(1, m_zmqObject->getState(VibePatt_amp)? Qt::darkGreen : Qt::red);
            return;
        }
        m_widgetMap[key]->setData(1, Qt::DisplayRole, QVariant(m_zmqObject->getLastValue(key)));
        if(findKey(m_DATA_SETPOINT,key)){
            m_widgetMap[key]->setTextColor(1, m_zmqObject->getState(key)? Qt::darkGreen : Qt::red);
        }
    });
}

QCasuTreeItem::~QCasuTreeItem()
{
    disconnect(m_zmqObjectConnection);
}
