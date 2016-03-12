#include "casusceneitem.h"

CasuSceneItem::CasuSceneItem(QObject *parent, int x, int y, CasuTreeItem *widget) : QObject(parent)
{
    x_center = x;
    y_center = y;

    this->setFlag(QGraphicsItem::ItemIsSelectable);

    widget_= widget;

    connect(widget_, SIGNAL(updateScene()), this, SLOT(updateScene()));
}

QRectF CasuSceneItem::boundingRect() const
{
    return QRectF(x_center-10,y_center-10,20,20);
}

void CasuSceneItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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
        widget_->setSelected(false);
    }
    if(widget_->led_on)brush.setColor(widget_->led_color);
    else brush.setColor(Qt::gray);

    if(widget_->isSelected())brush.setStyle(Qt::SolidPattern);
    else brush.setStyle(Qt::Dense3Pattern);

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(model);
}

void CasuSceneItem::updateScene(){
    this->scene()->update();
}

