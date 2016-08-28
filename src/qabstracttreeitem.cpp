#include "qabstracttreeitem.h"
#include "qabstractsceneitem.h"

void QAbstractTreeItem::resetSelection(){
    for(int k=0;k<this->childCount();k++)
        for(int i=0;i<this->child(k)->childCount();i++)
            this->child(k)->child(i)->setSelected(false);
}

void QAbstractTreeItem::setSceneItem(QGraphicsItem *sceneItem)
{
    _sceneItem = sceneItem;
}

bool QAbstractTreeItem::isChildSelected(){
    bool childSelected = false;
    for(int k = 0; k < 14; k++) childSelected |= _widgetMap[static_cast<dataType>(k)]->isSelected();
    return childSelected;
}

QList<zmqBuffer *> QAbstractTreeItem::getBuffers(){
    QList<zmqBuffer *> outList;
    for(int k = 0; k < 14; k++)
        if(_widgetMap[static_cast<dataType>(k)]->isSelected())
            outList.append((dynamic_cast<QAbstractSceneItem *>(_sceneItem))->getBuffers(static_cast<dataType>(k)));
    return outList;
}
