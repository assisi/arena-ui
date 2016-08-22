#include "qcolorbar.h"

QColorbar::QColorbar(QGraphicsItem* parent) : QGraphicsItem(parent)
{
}

QRectF QColorbar::boundingRect() const
{
    QGraphicsView* _view = this->scene()->views().first();
    // QSizeF is divided by transfromation factor so it remains the same size
    return QRectF(_view->mapToScene(QPoint(10,10)),QSizeF(180,15)/_view->transform().m11());
}

void QColorbar::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(!settings->value("temp_on").toBool()) return; //don't draw when not showing temp sensors

    QGraphicsView* _view = this->scene()->views().first();
    double scale = _view->transform().m11();

    QRectF model = boundingRect();

    QLinearGradient heatmap(_view->mapToScene(QPoint(10,10)),_view->mapToScene(QPoint(190,10)));
    heatmap.setColorAt(0, Qt::blue);
    heatmap.setColorAt(0.33, Qt::magenta);
    heatmap.setColorAt(0.66, Qt::red);
    heatmap.setColorAt(1, Qt::yellow);
    QPen pen(Qt::transparent);
    QBrush brush(heatmap);

    pen.setColor(Qt::transparent);

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRect(boundingRect());

    pen.setColor(Qt::black);
    pen.setWidth(0);
    painter->setPen(pen);

    painter->drawLine(_view->mapToScene(QPoint(10,25)),_view->mapToScene(QPoint(10,30)));
    painter->drawLine(_view->mapToScene(QPoint(100,25)),_view->mapToScene(QPoint(100,30)));
    painter->drawLine(_view->mapToScene(QPoint(190,25)),_view->mapToScene(QPoint(190,30)));

    QFont font = painter->font();
    font.setPointSizeF(11 / scale);
    painter->setFont(font);
    painter->drawText(_view->mapToScene(QPoint(11,36)), "20 °C");
    painter->drawText(_view->mapToScene(QPoint(101,36)), "35 °C");
    painter->drawText(_view->mapToScene(QPoint(191,36)), "50 °C");

}
