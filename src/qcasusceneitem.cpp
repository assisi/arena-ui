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
    Q_UNUSED(option)
    Q_UNUSED(widget)
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

    if(treeItem->child_selected)brush.setStyle(Qt::Dense3Pattern);
    else brush.setStyle(Qt::SolidPattern);

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(model);
    painter->drawLine(x_center + 5*cos(yaw_*PI/180),
                      y_center + 5*sin(yaw_*PI/180),
                      x_center + 10*cos(yaw_*PI/180),
                      y_center + 10*sin(yaw_*PI/180));

    pen.setStyle(Qt::SolidLine);
    brush.setStyle(Qt::SolidPattern);

    pen.setWidth(2);
    pen.setColor(Qt::transparent);
    painter->setPen(pen);

    //paint IR sensor readings
    if(settings->value("IR_on").toBool()){
        for(int k = 0; k < 6; k++){
            if(treeItem->connected){
                double tempGradient = treeItem->widget_IR_children[k]->data(1,Qt::DisplayRole).toDouble() / 5000;
                QColor tempColor;
                tempColor.setHsvF(0.14, 0,tempGradient);
                brush.setColor(tempColor);
            }
            else brush.setColor(Qt::gray);
            painter->setBrush(brush);
            painter->drawPolygon(QIRTriangle(QPointF(x_center, y_center), yaw_ - k*60)); // 0° is at 3 o'clock, ccw direction
        }
    }

    //paint Temp sensor readings
    if(settings->value("temp_on").toBool()){
        for(int k = 0; k < 4; k++){
            if(treeItem->connected){
                double tempTemp = treeItem->widget_temp_children[k]->data(1,Qt::DisplayRole).toDouble();
                if (tempTemp > 40) tempTemp = 40;
                double tempGradient = (tempTemp - 20) / 20;
                tempGradient = ((240 + (int)(tempGradient * 180)) % 360); // / 360; // calculate color gradiend in HSV space 
                QColor tempColor;
                tempColor.setHsv(tempGradient, 255, 255);
                pen.setColor(tempColor);
            }
            else pen.setColor(Qt::gray);

            painter->setPen(pen);

            QTempArc arc(QPointF(x_center, y_center), yaw_ - k*90); // 0° is at 3 o'clock, ccw direction
            painter->drawArc(arc.rect, arc.start ,arc.span);
        }
    }

    //paint airflow marker
    if(settings->value("air_on").toBool() &&
            treeItem->connected &&
            treeItem->airflowON){
        pen.setColor(Qt::transparent);
        brush.setColor(QColor(250, 218, 94,64));
        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawEllipse(x_center-20,y_center-20,40,40);
    }
}

void QCasuSceneItem::updateScene(){
    this->scene()->update();
}



QIRTriangle::QIRTriangle(QPointF center, double angle)
{
    QPointF left, right, top;

    double side = 2/sqrt(3) * 7; // 10 is the altitude length
    double offset = 11; // offset from center of CASU

    double angleTop = angle * PI/180;
    double angleLeft = (angle - 30) * PI/180;
    double angleRight = (angle + 30) * PI/180;

    top = QPointF(center.x() + offset*cos(angleTop), center.y() + offset*sin(angleTop));
    left = top + QPointF(side*cos(angleLeft), side*sin(angleLeft));
    right = top + QPointF(side*cos(angleRight), side*sin(angleRight));

    *this << top << right << left;
}


QTempArc::QTempArc(QPointF center, double angle)
{
    double offset = settings->value("IR_on").toBool()? 42 : 30; // offset from center of CASU

    span = 50 * 16; //Qt angles are in increments of 1°/16
    start = (angle - 25) * 16;
    rect = QRectF(center.x()-offset/2, center.y()-offset/2, offset, offset);
}
