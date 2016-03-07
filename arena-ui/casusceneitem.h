#ifndef CASUSCENEITEM_H
#define CASUSCENEITEM_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>

#include "casutreeitem.h"


class CasuSceneItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

private:
    int x_center;
    int y_center;


public:
    CasuTreeItem* widget_;

    CasuSceneItem(QObject *parent, int x, int y, CasuTreeItem *widget);

    QRectF boundingRect() const;

    //overloading paint()
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected slots:
    void updateScene();

};

#endif // CASUSCENEITEM_H
