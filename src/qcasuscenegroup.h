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
    QVector<QPointF> _childCoords;

    QVector<QLineF> Prim(QVector<QPointF> list);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

public:
    bool isGroup() const;
    QList<zmqBuffer *> getBuffers(dataType key);
    void setGroupColor(QColor color);

    QCasuSceneGroup(QAbstractTreeItem *treeItem);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QPainterPath shape();
    QPainterPath completeShape();
};

#endif // QCASUSCENEGROUP_H
