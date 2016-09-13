#include "qabstracttreeitem.h"
#include "qabstractsceneitem.h"

using namespace zmqData;

void QAbstractTreeItem::resetSelection() const{
    for(int k=0;k<this->childCount();k++){
        for(int i=0;i<this->child(k)->childCount();i++){
            this->child(k)->child(i)->setSelected(false);
        }
    }
}

void QAbstractTreeItem::setSceneItem(QGraphicsItem *sceneItem)
{
    m_sceneItem = sceneItem;
}

bool QAbstractTreeItem::isChildSelected() const
{
    bool childSelected = false;
    for(int k = 0; k < m_IR_NUM + m_temp_NUM; k++){
        childSelected |= m_widgetMap[dCast(k)]->isSelected();
    }
    return childSelected;
}

QList<zmqBuffer *> QAbstractTreeItem::getBuffers() const
{
    QList<zmqBuffer *> outList;
    for(int k = 0; k < m_IR_NUM + m_temp_NUM; k++){
        if(m_widgetMap[dCast(k)]->isSelected()){
            outList.append((sCast(m_sceneItem))->getBuffers(dCast(k)));
        }
    }
    return outList;
}
