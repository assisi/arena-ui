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
    QPainterPath m_groupLine;
    QPainterPath m_groupShape;
    QVector<QPointF> m_childCoordinates;

    QVector<QLineF> Prim(const QVector<QPointF> &list);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

public:
    explicit QCasuSceneGroup() = default;

    bool isGroup() const Q_DECL_OVERRIDE;
    QList<zmqData::zmqBuffer *> getBuffers(zmqData::dataType key) const Q_DECL_OVERRIDE;
    QVector<QPointF> getCoordinateVector() const Q_DECL_OVERRIDE;
    void sendSetpoint(const QList<QByteArray> &message) const Q_DECL_OVERRIDE;
    void setGroupColor(const QColor &color) Q_DECL_OVERRIDE;
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    QPainterPath completeShape() const Q_DECL_OVERRIDE;

    void addToGroup(QGraphicsItem *item);
    void addToGroup(QList<QGraphicsItem *> itemList);
    void removeFromGroup(QGraphicsItem *item);
    void removeFromGroup(QList<QGraphicsItem *> itemList);
};

#endif // QCASUSCENEGROUP_H
