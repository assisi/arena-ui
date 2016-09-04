#include "qabstractsceneitem.h"
#include "qabstracttreeitem.h"

using namespace zmqData;

void QAbstractSceneItem::recursiveSetHidden(bool state)
{
    if (isGroup())
        for(auto& item : childItems()){
            sCast(item)->recursiveSetHidden(state);
        }
    else {
        m_treeItem->setHidden(state);
        if(state) tCast(m_treeItem)->resetSelection();
    }
}

QVariant QAbstractSceneItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value){
    if(change == QGraphicsItem::ItemSelectedHasChanged){
        recursiveSetHidden(!value.toBool());
        m_treeItem->setHidden(!value.toBool());
        if(!value.toBool()) tCast(m_treeItem)->resetSelection();
    }
    return QGraphicsItem::itemChange(change, value);
}

QAbstractSceneItem::QAbstractSceneItem() :
    m_inGroup(false),
    m_groupColor(Qt::black)
{
    this->setFlag(GraphicsItemFlag::ItemIsSelectable);
}

void QAbstractSceneItem::setInGroup(bool state){
    m_inGroup = state;
}

void QAbstractSceneItem::setTreeItem(QTreeWidgetItem *treeItem)
{
    m_treeItem = treeItem;
}

void QAbstractSceneItem::deleteTreeItem()
{
    delete m_treeItem;
}

void QAbstractSceneItem::setGroupColor(const QColor &color){
    m_groupColor = color;
}

QPainterPath QAbstractSceneItem::shape() const
{
    QPainterPath out;
    out.addRect(boundingRect());
    return out;
}

QPainterPath QAbstractSceneItem::completeShape() const
{
    return shape();
}
