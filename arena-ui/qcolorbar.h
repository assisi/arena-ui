#ifndef QCOLORBAR_H
#define QCOLORBAR_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QStaticText>

#include "globalHeader.h"

class QColorbar : public QObject, public QGraphicsItem
{
    Q_OBJECT

private:

public:

    QColorbar(QObject *parent = 0);

    QRectF boundingRect() const;

    //overloading paint()
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};


#endif // QCOLORBAR_H
