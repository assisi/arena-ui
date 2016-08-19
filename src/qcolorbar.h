#ifndef QCOLORBAR_H
#define QCOLORBAR_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QStaticText>

#include "globalHeader.h"

/*!
 * \brief Graphics item which shows temperature color legend
 */
class QColorbar : public QGraphicsItem
{
private:

public:

    QColorbar(QGraphicsItem *parent = 0);

    QRectF boundingRect() const;

    //overloading paint()
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};


#endif // QCOLORBAR_H
