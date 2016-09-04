#include "qcasutreegroup.h"
#include "qabstractsceneitem.h"

using namespace zmqData;

QCasuTreeGroup::QCasuTreeGroup(QString name) :
    m_name(name)
{
    this->setData(0,Qt::DisplayRole,QStringList(m_name));
    QTreeWidgetItem* tempWidget;

    /* IR branch children */{
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
        for(int k = 0; k < m_temp_NUM; k++){
            m_widgetMap.insert(dCast(k + m_IR_NUM), tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        this->addChild(tempWidget);
    }
}


QSelectionTreeItem::QSelectionTreeItem(QGraphicsScene *scene) :
    QCasuTreeGroup(QString("Selected CASUS")),
    m_scene(scene)
{
}

QList<zmqBuffer *> QSelectionTreeItem::getBuffers() const
{
    QList<zmqBuffer *> outList;
    for(int k = 0; k < m_IR_NUM + m_temp_NUM; k++){
        if(m_widgetMap[dCast(k)]->isSelected()){
            for(auto& item : m_scene->selectedItems()){
                outList.append((sCast(item))->getBuffers(dCast(k)));
            }
        }
    }
    return outList;
}
