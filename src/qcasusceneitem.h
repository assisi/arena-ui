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

//ANIMATION
    double airflowAngle;

public:
    QCasuTreeItem* treeItem;

    QCasuSceneItem(QObject *parent, int x, int y, int yaw, QCasuTreeItem *widget);

    QRectF boundingRect() const;

    //overloading paint()
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected slots:
    void updateScene();

};

class QIRTriangle : public QRectF
{
public:
    QIRTriangle(QPointF center, double angle, double value);
};

class QTempArc
{
public:
    QTempArc(QPointF center, double angle);
    int start;
    int span;
    QRectF rect;
};

class QPetal : public QPainterPath
{
public:
    QPetal(QPointF center, double angle);
};

#endif // QCASUSCENEITEM_H
