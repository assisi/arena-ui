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
    for(int k = 0; k < _IR_num + _Temp_num; k++) childSelected |= _widgetMap[dCast(k)]->isSelected();
    return childSelected;
}

QList<zmqBuffer *> QAbstractTreeItem::getBuffers(){
    QList<zmqBuffer *> outList;
    for(int k = 0; k < _IR_num + _Temp_num; k++)
        if(_widgetMap[dCast(k)]->isSelected())
            outList.append((sCast(_sceneItem))->getBuffers(dCast(k)));
    return outList;
}
