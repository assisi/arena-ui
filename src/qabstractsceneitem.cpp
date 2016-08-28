#include "qabstractsceneitem.h"
#include "qabstracttreeitem.h"


QVariant QAbstractSceneItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value){
    if(change == QGraphicsItem::ItemSelectedHasChanged){
        _treeItem->setHidden(!value.toBool());
        if(!value.toBool()) dynamic_cast<QAbstractTreeItem* >(_treeItem)->resetSelection();
    }
    return QGraphicsItem::itemChange(change, value);
}

void QAbstractSceneItem::setInGroup(bool state){
    _inGroup = state;
}

void QAbstractSceneItem::setTreeItem(QTreeWidgetItem *treeItem)
{
    _treeItem = treeItem;
}

void QAbstractSceneItem::setGroupColor(QColor color){
    _groupColor = color;
}
