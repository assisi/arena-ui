#ifndef QCOLORBAR_H
#define QCOLORBAR_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QStaticText>
#include <QDebug>

#include "globalHeader.h"

/*!
 * \brief Graphics item which shows temperature color legend
 *
 * Item is always show in same place no matter what part of scene viewport is showing.
 */
class QColorbar : public QGraphicsItem
{
private:

public:

    QColorbar(QGraphicsItem *parent = 0);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};


#endif // QCOLORBAR_H
