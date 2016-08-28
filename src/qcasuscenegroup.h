#ifndef QCASUSCENEGROUP_H
#define QCASUSCENEGROUP_H

#include <QGraphicsItemGroup>
#include <QPen>
#include <QPainter>
#include <QVariant>
#include <QtAlgorithms>

#include "qabstractsceneitem.h"

class QCasuSceneGroup : public QAbstractSceneItem
{
private:
    QPainterPath _groupLine;
    QPainterPath _groupShape;
    QVector<QPointF> _childCoordinates;

    QVector<QLineF> Prim(QVector<QPointF> list);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

public:
    bool isGroup() const;
    QList<zmqBuffer *> getBuffers(dataType key);
    QVector<QPointF> getCoordinateVector();
    void sendSetpoint(QList<QByteArray> message);
    void setGroupColor(QColor color);

    QCasuSceneGroup();

    QRectF boundingRect();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QPainterPath shape() const;
    QPainterPath completeShape();
};

#endif // QCASUSCENEGROUP_H
