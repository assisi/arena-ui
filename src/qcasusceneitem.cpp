#include "qcasusceneitem.h"
#include "qabstracttreeitem.h"

bool QCasuSceneItem::isGroup() const
{
    return false;
}

QList<zmqBuffer *> QCasuSceneItem::getBuffers(dataType key)
{
    QList<zmqBuffer *> out;
    out.append(_zmqObject->getBuffer(key));
    return out;
}

QVector<QPointF> QCasuSceneItem::getCoordinateVector()
{
    QVector<QPointF> out;
    out.append(_coordinates);
    return out;
}

void QCasuSceneItem::sendSetpoint(QList<QByteArray> message)
{
    _zmqObject->sendSetpoint(message);
}

QCasuSceneItem::QCasuSceneItem(QPointF coordinates, double yaw, QCasuZMQ *zmqObject) :
    _coordinates(coordinates),
    _yaw((int)(yaw*180/PI)),
    _airflowAngle(0),
    _vibrAngle(0),
    _zmqObject(zmqObject)
{
    FPScheck = new QElapsedTimer();
}

void QCasuSceneItem::setAddresses(QStringList addresses)
{
    _zmqObject->setAddresses(addresses.at(0), addresses.at(1), addresses.at(2));
}

QStringList QCasuSceneItem::getAddresses()
{
    return _zmqObject->getAddresses();
}

QString QCasuSceneItem::getName()
{
    return _zmqObject->getName();
}

double QCasuSceneItem::getValue(dataType key)
{
    return _zmqObject->getValue(key);
}

bool QCasuSceneItem::getState(dataType key)
{
    return _zmqObject->getState(key);
}

bool QCasuSceneItem::isConnected()
{
    return _zmqObject->isConnected();
}

double QCasuSceneItem::getAvgSamplingTime()
{
    return _zmqObject->getAvgSamplingTime();
}


QRectF QCasuSceneItem::boundingRect() const
{
    return QRectF(_coordinates.x()-10,_coordinates.y()-10,20,20);
}

void QCasuSceneItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    bool FPSrepaint = FPScheck->elapsed() < 30 ? false : true;

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
        for(int k = 0; k < _IR_num; k++){
            brush.setColor(Qt::black);
            painter->setBrush(brush);
            double tempIR;
            // Scale sensor reading pie to be ~18cm (~3x the edge of CASU ring) at maximum reading value (2^16)
            if (_zmqObject->isConnected()) tempIR = _zmqObject->getValue(static_cast<dataType>(k)) / 65536 * 1.8;
            else tempIR = 0;
            painter->drawPie(QIRTriangle(_coordinates,_yaw + k*60, tempIR), (_yaw + k*60 - 25)*16, 50*16); // 0° is at 3 o'clock, ccw direction
        }
    }

    //paint Temp sensor readings
    if(settings->value("temp_on").toBool()){
        for(int k = 0; k < 4; k++){
            if(_zmqObject->isConnected()){
                double tempTemp = _zmqObject->getValue(static_cast<dataType>(6 + k));
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

            QTempArc arc(_coordinates, _yaw - k*90); // 0° is at 3 o'clock, ccw direction
            painter->drawArc(arc.rect, arc.start ,arc.span);
        }
    }

    // - WORKAROUND - drawing with white fill so seethrough patter doesnt show underlaying drawings
    pen.setWidth(0);
    brush.setColor(Qt::white);
    brush.setStyle(Qt::SolidPattern);
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(boundingRect());

    // - Configurating pen and brush parameters
    pen.setWidth(2);
    if(isSelected() && _inGroup)pen.setColor(_groupColor);
    else if(_zmqObject->isConnected())pen.setColor(Qt::green);
    else pen.setColor(Qt::red);

    if(isSelected()) pen.setStyle(Qt::DotLine);
    else pen.setStyle(Qt::SolidLine);

    brush.setColor(_zmqObject->getState(LED) ? _zmqObject->getLedColor() : Qt::gray);
    if(dynamic_cast<QAbstractTreeItem *>(_treeItem)->isChildSelected()) brush.setStyle(Qt::Dense2Pattern);

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(boundingRect());
    painter->drawLine(_coordinates.x() + 5*cos(-_yaw*PI/180),
                      _coordinates.y() + 5*sin(-_yaw*PI/180),
                      _coordinates.x() + 9*cos(-_yaw*PI/180),
                      _coordinates.y() + 9*sin(-_yaw*PI/180));

    //paint airflow marker
    if(settings->value("air_on").toBool() && _zmqObject->isConnected() && _zmqObject->getState(Airflow)){
        double value = _zmqObject->getValue(Airflow);

        pen.setColor(Qt::transparent);
        brush.setColor(QColor(250, 218, 94, 96));
        painter->setPen(pen);
        painter->setBrush(brush);
        // 30 FPS, max_speed = 6 deg/frame -> w = 0.5 rpm
        // CURRENTLY THERE IS ONLY ONE INTENSITY, WHEN INTESITY RANGE WILL BE ENABLED, MAX_SPEED SHOULD BE 12
        _airflowAngle = fmod(_airflowAngle + value * 6 * FPSrepaint, 360);
        painter->drawPath(QPetal(_coordinates,_airflowAngle));       // petal 1
        painter->drawPath(QPetal(_coordinates,_airflowAngle + 120)); // petal 2
        painter->drawPath(QPetal(_coordinates,_airflowAngle - 120)); // petal 3
    }

    //paint vibration marker
    if(settings->value("vibr_on").toBool() && _zmqObject->isConnected() && _zmqObject->getState(Speaker)){
        double freq = _zmqObject->getValue(Frequency);
        double amplitude = _zmqObject->getValue(Amplitude);

        pen.setColor(QColor(255,255,255,96));
        pen.setWidth(2);
        pen.setStyle(Qt::SolidLine);
        brush.setColor(Qt::transparent);
        painter->setPen(pen);
        painter->setBrush(brush);
        // 30 FPS, max_speed = 12 deg/frame -> w = 1 rpm
        _vibrAngle = fmod(_vibrAngle - amplitude/100* 12*FPSrepaint, 360);
        // wawesNum = [6 .. 15]
        int wawesNum = 6+9*freq/1500;
        QVibratingCircle tempItem = QVibratingCircle(_coordinates, wawesNum, _vibrAngle);
        painter->drawPath(tempItem);
        pen.setColor(QColor(128,128,128,96));
        painter->setPen(pen);
        painter->drawPoints(tempItem.points, 20);
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

    setTopLeft(topLeft);
    setBottomRight(bottomRight);
}


QTempArc::QTempArc(QPointF center, double angle)
{
    double offset = 17.5; // offset from center of CASU

    span = 50 * 16; //Qt angles are in increments of 1°/16
    start = (angle - 25) * 16;
    rect = QRectF(center.x()-offset, center.y()-offset, offset*2, offset*2);
}

QPetal::QPetal(QPointF center, double angle){
    double leftAngle = (angle+45) * PI/180;
    double rightAngle = (angle-45) * PI/180;

    moveTo(center);
    cubicTo(center + QPointF(35*cos(leftAngle),-35*sin(leftAngle)),
                  center + QPointF(35*cos(rightAngle),-35*sin(rightAngle)),
                  center);
}

QVibratingCircle::QVibratingCircle(QPointF center, int waves, double angle){
    angle = angle * PI/180;

    moveTo(center + QPointF(16+2*sin(angle),0));

    for(int k=1; k <= 360; k++){
        double coordAngle = k*PI/180;
        double amp = 16 + 2*sin(angle + waves*coordAngle);
        lineTo(center + QPointF(amp*cos(coordAngle),amp*sin(coordAngle)));
        if((k+9)%18 == 0) points[(k+9)/18-1] = center + QPointF(amp*cos(coordAngle),amp*sin(coordAngle));
    }

}
