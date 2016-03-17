#include "qcasusceneitem.h"

QCasuSceneItem::QCasuSceneItem(QObject *parent, int x, int y, QCasuTreeItem *widget) : QObject(parent),
    x_center(x),
    y_center(y),
    widget_(widget)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable);

    connect(widget_, SIGNAL(updateScene()), this, SLOT(updateScene()));
}


QRectF QCasuSceneItem::boundingRect() const
{
    return QRectF(x_center-10,y_center-10,20,20);
}

void QCasuSceneItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF model = boundingRect();

    QPen pen;
    QBrush brush;

    pen.setWidth(2);
    if(widget_->connected)pen.setColor(Qt::green);
    else pen.setColor(Qt::red);


    if(this->isSelected()){
        pen.setStyle(Qt::SolidLine);
        widget_->setHidden(false);
    }
    else{
        pen.setStyle(Qt::DotLine);
        widget_->setHidden(true);
        widget_->resetSelection();
    }
    if(widget_->led_on)brush.setColor(widget_->led_color);
    else brush.setColor(Qt::gray);

    if(widget_->child_selected)brush.setStyle(Qt::SolidPattern);
    else brush.setStyle(Qt::Dense3Pattern);

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(model);
}

void QCasuSceneItem::updateScene(){
    this->scene()->update();
}

