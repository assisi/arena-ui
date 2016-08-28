#include "qcasutreegroup.h"

QCasuTreeGroup::QCasuTreeGroup(QGraphicsItem *sceneItem) :
    QAbstractTreeItem(sceneItem),
    _name("CASU group")
{
    this->setData(0,Qt::DisplayRole,QStringList(_name));
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
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - F")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - L")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - B")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - R")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - TOP")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - PCB")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - RING")));
        tempWidget->addChild(new QTreeWidgetItem(QStringList("Temp - WAX")));
        for(int k = 0; k < 8; k++){
            _widgetMap.insert(static_cast<QCasuZMQ::dataType>(k + 6), tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        this->addChild(tempWidget);
    }
}


QSelectionTreeItem::QSelectionTreeItem(QGraphicsScene *scene) :
    QCasuTreeGroup(nullptr),
    _name("Selected CASUs"),
    _scene(scene)
{
}

QList<zmqBuffer *> QSelectionTreeItem::getBuffers()
{
    QList<zmqBuffer *> outList;
    for(int k = 0; k < 14; k++)
        if(_widgetMap[static_cast<dataType>(k)]->isSelected())
            foreach (QGraphicsItem* item, _scene->selectedItems())
                outList.append((dynamic_cast<QAbstractSceneItem *>(item))->getBuffers(static_cast<dataType>(k)));
    return outList;
}
