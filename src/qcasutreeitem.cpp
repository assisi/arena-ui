#include "qcasutreeitem.h"

QCasuTreeItem::QCasuTreeItem(QCasuZMQ *zmqObject) :
    _zmqObject(zmqObject)
{
    this->setData(0,Qt::DisplayRole,QStringList(_zmqObject->getName()));
    QTreeWidgetItem* tempWidget;

    //zadavanje djece IR grani:
    {
        tempWidget = new QTreeWidgetItem(QStringList("IR - Proximity"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("IR - F"), name + ": IR - F"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("IR - FL"), name + ": IR - FL"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("IR - BL"), name + ": IR - BL"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("IR - B"), name + ": IR - B"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("IR - BR"), name + ": IR - BR"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("IR - FR"), name + ": IR - FR"));
        for(int k = 0; k < 6; k++){
            _widgetMap.insert(static_cast<QCasuZMQ::dataType>(k), tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        this->addChild(tempWidget);
    }

    //zadavanje djece temp grani:
    {
        tempWidget = new QTreeWidgetItem(QStringList("Temperature"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - F"), name + ": Temp - F"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - L"), name + ": Temp - L"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - B"), name + ": Temp - B"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - R"), name + ": Temp - R"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - TOP"), name + ": Temp - TOP"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - PCB"), name + ": Temp - PCB"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - RING"), name + ": Temp - RING"));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - WAX"), name + ": Temp - WAX"));
        for(int k = 0; k < 8; k++){
            _widgetMap.insert(static_cast<QCasuZMQ::dataType>(k + 6), tempWidget->child(k));
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
            _widgetMap.insert(static_cast<QCasuZMQ::dataType>(k + 14), tempWidget->child(k));
            tempWidget->setFlags(Qt::ItemIsSelectable);
        }

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
            _widgetMap.insert(static_cast<QCasuZMQ::dataType>(k + 17), tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsEnabled);
        }
        for(int k = 0; k < 2; k++){
            _widgetMap.insert(static_cast<QCasuZMQ::dataType>(k + 19), tempWidget->child(2)->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        this->addChild(tempWidget);
    }

    this->setFlags(Qt::ItemIsEnabled);

    connect (_zmqObject,SIGNAL(updated(QCasuZMQ::dataType),this,SLOT(updateData(QCasuZMQ::dataType));
}

bool QCasuTreeItem::isChildSelected()
{
    bool childSelected = false;
    for(int k = 0; k < 14; k++) childSelected |= _widgetMap[static_cast<QCasuZMQ::dataType>(k)]->isSelected();
}

void QCasuTreeItem::updateData(QCasuZMQ::dataType key)
{
    _widgetMap[key]->setData(1, Qt::DisplayRole, QVariant(_zmqObject->getValue(key)));
    if(key >= 14) _widgetMap[key]->setTextColor(1, _zmqObject->getState(key)? Qt::green : Qt::red);
}
