#include "qcasuscenegroup.h"

QCasuSceneGroup::QCasuSceneGroup(QGraphicsItem* parent, QCasuTreeItem* widget) : QGraphicsItemGroup(parent),
    groupColor(Qt::black),
    isTopLevel(true),
    treeItem(widget)
{
}

QRectF QCasuSceneGroup::boundingRect() const
{
    return childrenBoundingRect();
}


void QCasuSceneGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(this->isSelected()) treeItem->setHidden(false);
    else treeItem->setHidden(true);

    if(!this->isSelected() || !isTopLevel)return;

    QRectF offset = QRectF(boundingRect().x()-1,boundingRect().y()-1,boundingRect().width()+2,boundingRect().height()+2);


    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(groupColor);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawRoundedRect(offset,10,10);

}
