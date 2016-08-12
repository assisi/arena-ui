#include "qcasusceneitem.h"

QCasuSceneItem::QCasuSceneItem(QObject *parent, int x, int y, double yaw, QCasuTreeItem *widget) : QObject(parent),
    x_center(x),
    y_center(y),
    yaw_((int)(yaw*180/PI)),
    //ANIMATION
    airflowAngle(0),
    vibrAngle(0),
    //WIDGET
    treeItem(widget)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable);
    FPScheck = new QElapsedTimer();
}


QRectF QCasuSceneItem::boundingRect() const
{
    return QRectF(x_center-10,y_center-10,20,20);
}

void QCasuSceneItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    int FPSrepaint = FPScheck->elapsed() < 30 ? 0 : 1;

    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);

    QPen pen;
    QBrush brush;

    pen.setStyle(Qt::SolidLine);
    brush.setStyle(Qt::SolidPattern);

    pen.setWidth(2);
    pen.setColor(Qt::transparent);
    painter->setPen(pen);

    //paint IR sensor readings
    if(settings->value("IR_on").toBool()){
        for(int k = 0; k < 6; k++){
            brush.setColor(Qt::black);
            painter->setBrush(brush);
            double value;
            if(treeItem->connected) value = treeItem->widget_IR_children[k]->data(1,Qt::DisplayRole).toDouble() / 5000;
            else value = 0;
            painter->drawPie(QIRTriangle(QPointF(x_center, y_center),yaw_ + k*60, value), (yaw_ + k*60 - 25)*16, 50*16); // 0° is at 3 o'clock, ccw direction
        }
    }

    //paint Temp sensor readings
    if(settings->value("temp_on").toBool()){
        for(int k = 0; k < 4; k++){
            if(treeItem->connected){
                double tempTemp = treeItem->widget_temp_children[k]->data(1,Qt::DisplayRole).toDouble();
                if (tempTemp > 50) tempTemp = 50;
                if (tempTemp < 20) tempTemp = 20;

                double tempGradient = (tempTemp - 20) / 30;
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

    //paint main CASU object
    QRectF model = QRectF(x_center-10,y_center-10,20,20);;

    pen.setWidth(2);
    if(treeItem->connected)pen.setColor(Qt::green);
    else pen.setColor(Qt::red);


    if(this->isSelected()){
        pen.setStyle(Qt::DotLine);
        treeItem->setHidden(false);
    }
    else{
        pen.setStyle(Qt::SolidLine);
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
    painter->drawLine(x_center + 5*cos(-yaw_*PI/180),
                      y_center + 5*sin(-yaw_*PI/180),
                      x_center + 10*cos(-yaw_*PI/180),
                      y_center + 10*sin(-yaw_*PI/180));

    //paint airflow marker
    if(settings->value("air_on").toBool() && treeItem->connected && treeItem->airflowON){
        double value = treeItem->widget_setpoints_children[1]->data(1,Qt::DisplayRole).toDouble();

        pen.setColor(Qt::transparent);
        brush.setColor(QColor(250, 218, 94, 96));
        painter->setPen(pen);
        painter->setBrush(brush);
        airflowAngle = fmod(airflowAngle + value * 12 * FPSrepaint, 360); // 30 FPS, max_speed = 12 deg/frame -> w = 1 rpm
        painter->drawPath(QPetal(QPointF(x_center,y_center),airflowAngle));       // petal 1
        painter->drawPath(QPetal(QPointF(x_center,y_center),airflowAngle + 120)); // petal 2
        painter->drawPath(QPetal(QPointF(x_center,y_center),airflowAngle - 120)); // petal 3
    }

    //paint vibration marker
    if(settings->value("vibr_on").toBool() && treeItem->connected && treeItem->vibrON){
        double freq = treeItem->widget_setpoints_vibr_children[0]->data(1,Qt::DisplayRole).toDouble();
        double amplitude = treeItem->widget_setpoints_vibr_children[1]->data(1,Qt::DisplayRole).toDouble();

        pen.setColor(QColor(255,255,255,96));
        pen.setWidth(2);
        pen.setStyle(Qt::SolidLine);
        brush.setColor(Qt::transparent);
        painter->setPen(pen);
        painter->setBrush(brush);
        vibrAngle = fmod(vibrAngle - /*amplitude/100* */12*FPSrepaint, 360); // 30 FPS, max_speed = 12 deg/frame -> w = 1 rpm
        painter->drawPath(QVibratingCircle(QPointF(x_center,y_center), 6+9*freq/1500, vibrAngle));       // num_waves = [6 .. 15]
    }

    if(FPSrepaint) FPScheck->start();
}


QIRTriangle::QIRTriangle(QPointF center, double angle, double value)
{
    double side = 5+18*value; //
    double offset = 3; // center offset from center of CASU

    angle = angle * PI/180;
    center += QPointF(offset*cos(angle), -offset*sin(angle));
    QPointF topLeft = center - QPointF(side*sqrt(2),side*sqrt(2));
    QPointF bottomRight = center + QPointF(side*sqrt(2),side*sqrt(2));

    QRectF out(topLeft,bottomRight);

    this->setTopLeft(topLeft);
    this->setBottomRight(bottomRight);
}


QTempArc::QTempArc(QPointF center, double angle)
{
    double offset = settings->value("IR_on").toBool()? 42 : 30; // offset from center of CASU

    span = 50 * 16; //Qt angles are in increments of 1°/16
    start = (angle - 25) * 16;
    rect = QRectF(center.x()-offset/2, center.y()-offset/2, offset, offset);
}

QPetal::QPetal(QPointF center, double angle){
    double leftAngle = (angle+45) * PI/180;
    double rightAngle = (angle-45) * PI/180;

    this->moveTo(center);
    this->cubicTo(center + QPointF(35*cos(leftAngle),-35*sin(leftAngle)),
                  center + QPointF(35*cos(rightAngle),-35*sin(rightAngle)),
                  center);
}

QVibratingCircle::QVibratingCircle(QPointF center, int waves, double angle){
    angle = angle * PI/180;
    this->moveTo(center + QPointF(14+sin(angle),0));
    for(int k=1; k <= 360; k++){
        double coordAngle = k*PI/180;
        double amp = 16 + 2*sin(angle + waves*coordAngle);
        this->lineTo(center + QPointF(amp*cos(coordAngle),amp*sin(coordAngle)));
    }
}
