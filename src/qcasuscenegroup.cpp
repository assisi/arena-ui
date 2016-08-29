#include "qcasuscenegroup.h"
#include "qcasusceneitem.h"

QVariant QCasuSceneGroup::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    QAbstractSceneItem::itemChange(change,value);

    if(change==QGraphicsItem::ItemChildAddedChange || change==QGraphicsItem::ItemChildRemovedChange){
        QPainterPath newLine, newShape;

        _childCoordinates.clear();

        foreach (QGraphicsItem *item, childItems()) {
            foreach (QPointF point, dynamic_cast<QAbstractSceneItem *>(item)->getCoordinateVector())
                _childCoordinates.append(point);
            newShape.addPath(item->shape());
        }

        if(_childCoordinates.size()<2) return QGraphicsItem::itemChange(change, value);

        QVector<QLineF> mst = Prim(_childCoordinates);

        foreach (QLineF line, mst) {
            newLine.moveTo(line.p1());
            newLine.lineTo(line.p2());
        }

        _groupLine = newLine;
        _groupShape = newShape;
    }

    return QGraphicsItem::itemChange(change, value);
}

bool QCasuSceneGroup::isGroup() const
{
    return true;
}

QList<zmqBuffer *> QCasuSceneGroup::getBuffers(dataType key)
{
    QList<zmqBuffer *> out;
    foreach (QGraphicsItem *item, childItems()) {
        out.append(dynamic_cast<QAbstractSceneItem *>(item)->getBuffers(key));
    }
    return out;
}

QVector<QPointF> QCasuSceneGroup::getCoordinateVector()
{
    return _childCoordinates;
}

void QCasuSceneGroup::sendSetpoint(QList<QByteArray> message)
{
    foreach(QGraphicsItem *item, childItems())
        dynamic_cast<QAbstractSceneItem *>(item)->sendSetpoint(message);
}

void QCasuSceneGroup::setGroupColor(QColor color)
{
    _groupColor = color;
    _treeItem->setTextColor(0, _groupColor);
    foreach (QGraphicsItem *item, childItems())
        dynamic_cast<QAbstractSceneItem *>(item)->setGroupColor(color);
}

void QCasuSceneGroup::addToGroup(QGraphicsItem *item)
{
    QGraphicsItemGroup::addToGroup(item);
    dynamic_cast<QAbstractSceneItem *>(item)->setInGroup(true);
}

void QCasuSceneGroup::addToGroup(QList<QGraphicsItem *> itemList)
{
    foreach(QGraphicsItem* item, itemList) addToGroup(item);
}

void QCasuSceneGroup::removeFromGroup(QGraphicsItem *item)
{
    QGraphicsItemGroup::removeFromGroup(item);
    item->setSelected(false);
    item->setSelected(true);
    dynamic_cast<QAbstractSceneItem *>(item)->setInGroup(false);
}

void QCasuSceneGroup::removeFromGroup(QList<QGraphicsItem *> itemList)
{
    foreach(QGraphicsItem* item, itemList) removeFromGroup(item);
}

QCasuSceneGroup::QCasuSceneGroup()
{

}

QRectF QCasuSceneGroup::boundingRect()
{
    return childrenBoundingRect();
}

void QCasuSceneGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(!isSelected() || _inGroup)return;

    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);

    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(_groupColor);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawPath(_groupLine);
}

QPainterPath QCasuSceneGroup::shape() const
{
    return _groupShape;
}

QPainterPath QCasuSceneGroup::completeShape()
{
    QPainterPath tempShape = _groupShape;
    tempShape.addPath(_groupLine);
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
