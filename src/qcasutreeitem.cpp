#include "qcasutreeitem.h"

QCasuTreeItem::QCasuTreeItem(QCasuZMQ *zmqObject) :
    _zmqObject(zmqObject)
{
    setData(0,Qt::DisplayRole,QStringList(_zmqObject->getName()));
    QTreeWidgetItem* tempWidget;

    tempWidget = new QNoSortTreeItem(QStringList("LED color"));
    tempWidget->setFlags(Qt::ItemIsEnabled);
    _widgetMap.insert(LED, tempWidget);
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
        for(int k = 0; k < _IR_num; k++){
            _widgetMap.insert(dCast(k), tempWidget->child(k));
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
        for(int k = 0; k < _Temp_num; k++){
            _widgetMap.insert(dCast(k + _IR_num), tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        addChild(tempWidget);
    }
    //zadavanje djece vibr grani:
    /*{
        tempWidget = new customQTreeWidgetItem(QStringList("Vibration"));
        tempWidget->addChild(new customQTreeWidgetItem(QStringList("Frequency"), name + ": Vibration - freq"));
        tempWidget->addChild(new customQTreeWidgetItem(QStringList("Amplitude"), name + ": Vibration - amp"));
        tempWidget->addChild(new customQTreeWidgetItem(QStringList("StdDev"), name + ": Vibration - stdDev"));
        for(int k = 0; k < 3; k++){
            _widgetMap.insert(sCast(k + 14), tempWidget->child(k));
            tempWidget->setFlags(Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        addChild(tempWidget);
    } */
    //zadavanje djece setpoint grani:
    {
        tempWidget = new QNoSortTreeItem(QStringList("Current setpoints"));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Peltier")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Airflow")));
        tempWidget->addChild(new QNoSortTreeItem(QStringList("Speaker")));
        tempWidget->child(2)->addChild(new QNoSortTreeItem(QStringList("Frequency")));
        tempWidget->child(2)->addChild(new QNoSortTreeItem(QStringList("Amplitude")));
        for(int k = 0; k < 2; k++){
            _widgetMap.insert(dCast(k + 17), tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsEnabled);
        }
        for(int k = 0; k < 2; k++){
            _widgetMap.insert(dCast(k + 14), tempWidget->child(2)->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        addChild(tempWidget);
    }

    setFlags(Qt::ItemIsEnabled);

    /* BUG 73: this is sometimes called when object doesn't exist
     * - if simulator is running and project is opened over existing one
     */

    _zmqObjectConnection = connect (_zmqObject,&QCasuZMQ::updated,[&](dataType key){
        if(_widgetMap.isEmpty()) return;
        if(key == LED){
            if(_zmqObject->getState(LED)) _widgetMap[key]->setData(1, Qt::DisplayRole, QVariant(_zmqObject->getLedColor().name()));
            else _widgetMap[key]->setData(1, Qt::DisplayRole, QVariant());
            _widgetMap[key]->setTextColor(1, _zmqObject->getLedColor());
            return;
        }
        _widgetMap[key]->setData(1, Qt::DisplayRole, QVariant(_zmqObject->getValue(key)));
        if(key >= 14) _widgetMap[key]->setTextColor(1, _zmqObject->getState(key)? Qt::green : Qt::red);
    });
}

QCasuTreeItem::~QCasuTreeItem()
{
    disconnect();
}
