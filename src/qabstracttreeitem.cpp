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
    for(auto &key : m_DATA_BUFFERS){
        childSelected |= m_widgetMap[key]->isSelected();
    }
    return childSelected;
}

QList<zmqBuffer *> QAbstractTreeItem::getBuffers() const
{
    QList<zmqBuffer *> outList;
    for(auto &key : m_DATA_BUFFERS){
        if(m_widgetMap[key]->isSelected()){
            outList.append((sCast(m_sceneItem))->getBuffers(key));
        }
    }
    return outList;
}
