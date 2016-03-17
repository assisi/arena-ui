#ifndef QCASUSCENEITEM_H
#define QCASUSCENEITEM_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>

#include "qcasutreeitem.h"


class QCasuSceneItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

private:
    int x_center;
    int y_center;

public:
    QCasuTreeItem* treeItem;

    QCasuSceneItem(QObject *parent, int x, int y, QCasuTreeItem *widget);

    QRectF boundingRect() const;

    //overloading paint()
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected slots:
    void updateScene();

};

#endif // QCASUSCENEITEM_H
