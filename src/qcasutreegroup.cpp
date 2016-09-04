#include "qcasutreegroup.h"
#include "qabstractsceneitem.h"

using namespace zmqData;

QCasuTreeGroup::QCasuTreeGroup(QString name) :
    _name(name)
{
    this->setData(0,Qt::DisplayRole,QStringList(_name));
    QTreeWidgetItem* tempWidget;

    /* IR branch children */{
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
        this->addChild(tempWidget);
    }

    /* Temperature branch children */{
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
        this->addChild(tempWidget);
    }
}


QSelectionTreeItem::QSelectionTreeItem(QGraphicsScene *scene) :
    QCasuTreeGroup(QString("Selected CASUS")),
    _scene(scene)
{
}

QList<zmqBuffer *> QSelectionTreeItem::getBuffers()
{
    QList<zmqBuffer *> outList;
    for(int k = 0; k < _IR_num + _Temp_num; k++)
        if(_widgetMap[dCast(k)]->isSelected())
            for(auto& item : _scene->selectedItems())
                outList.append((sCast(item))->getBuffers(dCast(k)));
    return outList;
}
