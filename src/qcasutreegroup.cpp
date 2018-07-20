#include "qcasutreegroup.h"
#include "qabstractsceneitem.h"

using namespace zmqData;

QCasuTreeGroup::QCasuTreeGroup(QString name) :
    m_name(name)
{
    this->setData(0,Qt::DisplayRole,QStringList(m_name));
    this->setFlags(Qt::ItemIsEnabled);
    QTreeWidgetItem* tempWidget;

    /* IR branch children */{
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
        for(uint k = 0; k < m_TEMP_ARRAY.size(); k++){
            m_widgetMap.insert(m_TEMP_ARRAY[k], tempWidget->child(k));
            tempWidget->child(k)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
        tempWidget->setFlags(Qt::ItemIsEnabled);
        this->addChild(tempWidget);
    }
    /* Peltier */{
        tempWidget = new QNoSortTreeItem(QStringList("Peltier"));
        m_widgetMap.insert(Peltier, tempWidget);
        tempWidget->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        this->addChild(tempWidget);
    }
}


QSelectionTreeItem::QSelectionTreeItem(QGraphicsScene *scene) :
    QCasuTreeGroup(QString("Selected CASUS")),
    m_scene(scene)
{
}

QList<QSharedPointer<zmqBuffer> > QSelectionTreeItem::getBuffers() const
{
    QList<QSharedPointer<zmqBuffer> > outList;
    for(auto &key : m_DATA_BUFFERS){
        if(m_widgetMap[key]->isSelected()){
            for(auto& item : m_scene->selectedItems()){
                outList.append((sCast(item))->getBuffers(key));
            }
        }
    }
    return outList;
}
