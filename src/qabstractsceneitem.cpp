#include "qabstractsceneitem.h"
#include "qabstracttreeitem.h"


void QAbstractSceneItem::recursiveSetHidden(bool state)
{
    if (isGroup())
        foreach(QGraphicsItem *item, childItems())
            dynamic_cast<QAbstractSceneItem *>(item)->recursiveSetHidden(state);
    else {
        _treeItem->setHidden(state);
        if(state) dynamic_cast<QAbstractTreeItem* >(_treeItem)->resetSelection();
    }
}

QVariant QAbstractSceneItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value){
    if(change == QGraphicsItem::ItemSelectedHasChanged){
        recursiveSetHidden(!value.toBool());
        _treeItem->setHidden(!value.toBool());
        if(!value.toBool()) dynamic_cast<QAbstractTreeItem *>(_treeItem)->resetSelection();
    }
    return QGraphicsItem::itemChange(change, value);
}

QAbstractSceneItem::QAbstractSceneItem() :
    _inGroup(false),
    _groupColor(Qt::black)
{
    this->setFlag(GraphicsItemFlag::ItemIsSelectable);
}

void QAbstractSceneItem::setInGroup(bool state){
    _inGroup = state;
}

void QAbstractSceneItem::setTreeItem(QTreeWidgetItem *treeItem)
{
    _treeItem = treeItem;
}

void QAbstractSceneItem::deleteTreeItem()
{
    delete _treeItem;
}

void QAbstractSceneItem::setGroupColor(QColor color){
    _groupColor = color;
}

QPainterPath QAbstractSceneItem::shape() const
{
    QPainterPath out;
    out.addRect(boundingRect());
    return out;
}

QPainterPath QAbstractSceneItem::completeShape()
{
    return shape();
}
