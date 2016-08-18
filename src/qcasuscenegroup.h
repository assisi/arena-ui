#ifndef QCASUSCENEGROUP_H
#define QCASUSCENEGROUP_H

#include <QGraphicsItemGroup>
#include <QPen>
#include <QPainter>
#include <QVariant>
#include <QtAlgorithms>

#include "qcasutreeitem.h"
#include "qcasusceneitem.h"

class QCasuSceneGroup : public QGraphicsItemGroup
{
private:
    QPainterPath groupLine;
    QPainterPath groupShape;

    QColor groupColor;

    QVector<QLineF> Prim(QVector<QPointF> list);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

public:
    QVector<QPointF> childCoords;
    bool isTopLevel;
    QCasuTreeItem* treeItem;

    explicit QCasuSceneGroup(QGraphicsItem* parent = 0, QCasuTreeItem *widget = 0);
    ~QCasuSceneGroup();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setGroupColor(QColor color);

    QPainterPath shape() const;

    QPainterPath completeShape() const;
signals:

public slots:

};

#endif // QCASUSCENEGROUP_H
