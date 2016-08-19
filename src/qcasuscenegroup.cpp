#include "qcasuscenegroup.h"

QVariant QCasuSceneGroup::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change==QGraphicsItem::ItemChildAddedChange || change==QGraphicsItem::ItemChildRemovedChange){
        QPainterPath newLine, newShape;

        childCoords.clear();

        foreach (QGraphicsItem* item, childItems()) {
            if(item->childItems().size())
                foreach (QPointF subItem, ((QCasuSceneGroup*)item)->childCoords)
                    childCoords.append(subItem);
            else childCoords.append(QPointF(((QCasuSceneItem*)item)->x_center,((QCasuSceneItem*)item)->y_center));
            newShape.addPath(item->shape());
        }

        if(childCoords.size()<2)return QGraphicsItem::itemChange(change, value);

        QVector<QLineF> mst = Prim(childCoords);

        foreach (QLineF line, mst) {
            newLine.moveTo(line.p1());
            newLine.lineTo(line.p2());
        }

        groupLine = newLine;
        groupShape = newShape;
    }

    return QGraphicsItem::itemChange(change, value);
}

QCasuSceneGroup::QCasuSceneGroup(QGraphicsItem* parent, QCasuTreeItem* widget) : QGraphicsItemGroup(parent),
    groupColor(Qt::black),
    isTopLevel(true),
    treeItem(widget)
{
}

QCasuSceneGroup::~QCasuSceneGroup()
{
    delete treeItem;
}

QRectF QCasuSceneGroup::boundingRect() const
{
    return childrenBoundingRect();
}


void QCasuSceneGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(this->isSelected() && isTopLevel)treeItem->setHidden(false);
    else treeItem->setHidden(true);

    if(!this->isSelected() || !isTopLevel)return;

    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(groupColor);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawPath(groupLine);
}

void QCasuSceneGroup::setGroupColor(QColor color)
{
    groupColor = color;
    foreach (QGraphicsItem* item, childItems())
        if(item->childItems().size()) ((QCasuSceneGroup*)item)->setGroupColor(color);
        else ((QCasuSceneItem*)item)->groupColor = color;

}

QPainterPath QCasuSceneGroup::shape() const
{
    return groupShape;
}

QPainterPath QCasuSceneGroup::completeShape() const
{
    QPainterPath tempShape = groupShape;
    tempShape.addPath(groupLine);
    return tempShape;
}

bool lineCompare(QLineF a, QLineF b){return a.length()<b.length();}
QVector<QLineF> QCasuSceneGroup::Prim(QVector<QPointF> list)
{
    QVector<QLineF> allLines;
    QVector<QLineF> mst;
    QVector<QPointF> visited;

    for(int k = 0; k<list.size(); k++)
        for(int i = k+1; i<list.size(); i++)
            allLines.append(QLineF(list[k],list[i]));
    qSort(allLines.begin(),allLines.end(),lineCompare);

    mst.append(allLines.first());
    visited.append(allLines.first().p1());
    visited.append(allLines.first().p2());

    while(visited.size() != list.size()){
        QLineF next(QPointF(0,0),QPointF(800,800));
        foreach (QLineF line, allLines) {
            int i1 = visited.indexOf(line.p1());
            int i2 = visited.indexOf(line.p2());
            if((2*i1+1)*(2*i2+1) > 0) continue;
            if (line.length()>next.length()) continue;
            next = line;
        }

        mst.append(next);
        if(visited.indexOf(next.p1()) == -1) visited.append(next.p1());
        else visited.append(next.p2());
    }

    return mst;
}
