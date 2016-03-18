#include "qcasusceneitem.h"

QCasuSceneItem::QCasuSceneItem(QObject *parent, int x, int y, int yaw, QCasuTreeItem *widget) : QObject(parent),
    x_center(x),
    y_center(y),
    yaw_(yaw),
    treeItem(widget)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable);

    connect(treeItem, SIGNAL(updateScene()), this, SLOT(updateScene()));
}


QRectF QCasuSceneItem::boundingRect() const
{
    return QRectF(x_center-10,y_center-10,20,20);
}

void QCasuSceneItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //paint main CASU object
    QRectF model = QRectF(x_center-10,y_center-10,20,20);;

    QPen pen;
    QBrush brush;

    pen.setWidth(2);
    if(treeItem->connected)pen.setColor(Qt::green);
    else pen.setColor(Qt::red);


    if(this->isSelected()){
        pen.setStyle(Qt::SolidLine);
        treeItem->setHidden(false);
    }
    else{
        pen.setStyle(Qt::DotLine);
        treeItem->setHidden(true);
        treeItem->resetSelection();
    }
    if(treeItem->led_on)brush.setColor(treeItem->led_color);
    else brush.setColor(Qt::gray);

    if(treeItem->child_selected)brush.setStyle(Qt::SolidPattern);
    else brush.setStyle(Qt::Dense3Pattern);

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(model);

    //paint IR sensor readings
    //paint Temp sensor readings

}

void QCasuSceneItem::updateScene(){
    this->scene()->update();
}

