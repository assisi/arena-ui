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
    QCasuSceneGroup() = default;

    bool isGroup() const Q_DECL_OVERRIDE;
    QList<zmqBuffer *> getBuffers(dataType key) Q_DECL_OVERRIDE;
    QVector<QPointF> getCoordinateVector() Q_DECL_OVERRIDE;
    void sendSetpoint(QList<QByteArray> message) Q_DECL_OVERRIDE;
    void setGroupColor(QColor color) Q_DECL_OVERRIDE;
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    QPainterPath completeShape() Q_DECL_OVERRIDE;

    void addToGroup(QGraphicsItem *item);
    void addToGroup(QList<QGraphicsItem *> itemList);
    void removeFromGroup(QGraphicsItem *item);
    void removeFromGroup(QList<QGraphicsItem *> itemList);
};

#endif // QCASUSCENEGROUP_H
