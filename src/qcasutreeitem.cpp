#include "qcasutreeitem.h"

QCasuTreeItem::QCasuTreeItem(QCasuZMQ *zmqObject) :
    _zmqObject(zmqObject)
{
    this->setData(0,Qt::DisplayRole,QStringList(_zmqObject->getName()));
    QTreeWidgetItem* tempWidget;

    //zadavanje djece IR grani:
    {
        tempWidget = new QTreeWidgetItem(QStringList("IR - Proximity"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("IR - F")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("IR - FL")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("IR - BL")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("IR - B")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("IR - BR")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("IR - FR")));
        for(int k = 0; k < _IR_num; k++){
            _widgetMap.insert(static_cast<dataType>(k), tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        this->addChild(tempWidget);
    }

    //zadavanje djece temp grani:
    {
        tempWidget = new QTreeWidgetItem(QStringList("Temperature"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - F")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - L")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - B")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - R")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - TOP")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - PCB")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - RING")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - WAX")));
        for(int k = 0; k < _Temp_num; k++){
            _widgetMap.insert(static_cast<dataType>(k + _IR_num), tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        this->addChild(tempWidget);
    }
    //zadavanje djece vibr grani:
    /*{
        tempWidget = new QTreeWidgetItem(QStringList("Vibration"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Frequency"), name + ": Vibration - freq"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Amplitude"), name + ": Vibration - amp"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("StdDev"), name + ": Vibration - stdDev"));
        for(int k = 0; k < 3; k++){
            _widgetMap.insert(static_cast<dataType>(k + 14), tempWidget->child(k));
            tempWidget->setFlags(Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        this->addChild(tempWidget);
    } */
    //zadavanje djece setpoint grani:
    {
        tempWidget = new QTreeWidgetItem(QStringList("Current setpoints"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Peltier")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Airflow")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Speaker")));
        tempWidget->child(2)->addChild(new QTreeWidgetItem(QStringList("Frequency")));
        tempWidget->child(2)->addChild(new QTreeWidgetItem(QStringList("Amplitude")));
        for(int k = 0; k < 2; k++){
            _widgetMap.insert(static_cast<dataType>(k + 17), tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsEnabled);
        }
        for(int k = 0; k < 2; k++){
            _widgetMap.insert(static_cast<dataType>(k + 14), tempWidget->child(2)->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        this->addChild(tempWidget);
    }

    this->setFlags(Qt::ItemIsEnabled);

    connect (_zmqObject,SIGNAL(updated(dataType)),this,SLOT(updateData(dataType)));
}


void QCasuTreeItem::updateData(dataType key)
{
    _widgetMap[key]->setData(1, Qt::DisplayRole, QVariant(_zmqObject->getValue(key)));
    if(key >= 14) _widgetMap[key]->setTextColor(1, _zmqObject->getState(key)? Qt::green : Qt::red);
}
