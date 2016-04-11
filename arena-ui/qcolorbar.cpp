#include "qcolorbar.h"

QColorbar::QColorbar(QObject* parent) : QObject(parent)
{
}

QRectF QColorbar::boundingRect() const
{
    return QRectF(10,750,180,30);
}

void QColorbar::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(!settings->value("temp_on").toBool()) return; //don't draw when not showing temp sensors

    QRectF model = this->boundingRect();

    QLinearGradient heatmap(10,770,190,770);
    heatmap.setColorAt(0, Qt::blue);
    heatmap.setColorAt(0.33, Qt::magenta);
    heatmap.setColorAt(0.66, Qt::red);
    heatmap.setColorAt(1, Qt::yellow);
    QPen pen(Qt::transparent);
    QBrush brush(heatmap);

    pen.setColor(Qt::transparent);

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRect(model);

    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->drawLine(10,780,10,785);
    painter->drawLine(100,780,100,785);
    painter->drawLine(190,780,190,785);

    painter->drawStaticText(10,783, QStaticText("20 °C"));
    painter->drawStaticText(100,783, QStaticText("30 °C"));
    painter->drawStaticText(190,783, QStaticText("40 °C"));

}
