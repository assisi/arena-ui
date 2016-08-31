#include "qcasuscenegroup.h"
#include "qcasusceneitem.h"

QVariant QCasuSceneGroup::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    QAbstractSceneItem::itemChange(change,value);

    if(change==QGraphicsItem::ItemChildAddedChange || change==QGraphicsItem::ItemChildRemovedChange){
        QPainterPath newLine, newShape;

        _childCoordinates.clear();

        for(auto& item : childItems()) {
            for(auto& point : sCast(item)->getCoordinateVector())
                _childCoordinates.append(point);
            newShape.addPath(item->shape());
        }

        if(_childCoordinates.size()<2) return QGraphicsItem::itemChange(change, value);

        QVector<QLineF> mst = Prim(_childCoordinates);

        for (QLineF& line : mst) {
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
    for(auto& item : childItems())
        out.append(sCast(item)->getBuffers(key));
    return out;
}

QVector<QPointF> QCasuSceneGroup::getCoordinateVector()
{
    return _childCoordinates;
}

void QCasuSceneGroup::sendSetpoint(QList<QByteArray> message)
{
    for(auto& item : childItems())
        sCast(item)->sendSetpoint(message);
}

void QCasuSceneGroup::setGroupColor(QColor color)
{
    _groupColor = color;
    _treeItem->setTextColor(0, _groupColor);
    for(auto& item : childItems())
        sCast(item)->setGroupColor(color);
}

void QCasuSceneGroup::addToGroup(QGraphicsItem *item)
{
    QGraphicsItemGroup::addToGroup(item);
    sCast(item)->setInGroup(true);
}

void QCasuSceneGroup::addToGroup(QList<QGraphicsItem *> itemList)
{
    for(auto& item : itemList) addToGroup(item);
}

void QCasuSceneGroup::removeFromGroup(QGraphicsItem *item)
{
    QGraphicsItemGroup::removeFromGroup(item);
    item->setSelected(false);
    item->setSelected(true);
    sCast(item)->setInGroup(false);
}

void QCasuSceneGroup::removeFromGroup(QList<QGraphicsItem *> itemList)
{
    for(auto& item : itemList) removeFromGroup(item);
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

QVector<QLineF> QCasuSceneGroup::Prim(QVector<QPointF> list)
{
    QVector<QLineF> allLines;
    QVector<QLineF> mst;
    QVector<QPointF> visited;

    // NOTE: experimental nested range-for
    for(auto& point1 : list)
        for(auto& point2 : list.mid(&point1-&list.first()+1))
            allLines.append(QLineF(point1,point2));

    qSort(allLines.begin(),allLines.end(),[](QLineF a, QLineF b){return a.length()<b.length();});

    mst.append(allLines.first());
    visited.append(allLines.first().p1());
    visited.append(allLines.first().p2());

    while(visited.size() != list.size()){
        QLineF next(QPointF(0,0),QPointF(800,800));
        for(auto& line : allLines) {
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
