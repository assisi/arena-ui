#ifndef QCASUSCENEITEM_H
#define QCASUSCENEITEM_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>

#include "qcasutreeitem.h"

#define PI 3.14159265

class QCasuSceneItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

private:
    int x_center;
    int y_center;
    int yaw_;

public:
    QCasuTreeItem* treeItem;

    QCasuSceneItem(QObject *parent, int x, int y, int yaw, QCasuTreeItem *widget);

    QRectF boundingRect() const;

    //overloading paint()
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected slots:
    void updateScene();

};

class QIRTriangle : public QPolygonF
{
public:
    QIRTriangle(QPointF center, double angle);
};

class QTempArc
{
public:
    QTempArc(QPointF center, double angle);
    int start;
    int span;
    QRectF rect;
};

#endif // QCASUSCENEITEM_H