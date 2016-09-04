#include "qcasusceneitem.h"
#include "qabstracttreeitem.h"

using namespace zmqData;

static const double PI = 3.14159265;

bool QCasuSceneItem::isGroup() const
{
    return false;
}

QList<zmqBuffer *> QCasuSceneItem::getBuffers(dataType key) const
{
    QList<zmqBuffer *> out;
    out.append(m_zmqObject->getBuffer(key));
    return out;
}

QVector<QPointF> QCasuSceneItem::getCoordinateVector() const
{
    QVector<QPointF> out;
    out.append(m_coordinates);
    return out;
}

void QCasuSceneItem::sendSetpoint(const QList<QByteArray> &message) const
{
    // from context menu until here, message is passed as const and by reference
    // QCasuZMQ has to add CASU name in front so message is passed by value (making a copy)
    m_zmqObject->sendSetpoint(message);
}

QCasuSceneItem::QCasuSceneItem(QPointF coordinates, double yaw, QCasuZMQ *zmqObject) :
    m_coordinates(coordinates),
    m_yaw((int)(yaw*180/PI)),
    m_airflowAngle(0),
    m_vibrAngle(0),
    m_zmqObject(zmqObject)
{
    FPScheck = new QElapsedTimer();
}

QCasuZMQ *QCasuSceneItem::getZmqObject()
{
    return m_zmqObject;
}

QRectF QCasuSceneItem::boundingRect() const
{
    return QRectF(m_coordinates.x()-10, m_coordinates.y()-10,20,20);
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
    if(g_settings->value("IR_on").toBool()){
        for(int k = 0; k < m_IR_NUM; k++){
            brush.setColor(Qt::black);
            painter->setBrush(brush);
            double tempIR;

            if (m_zmqObject->isConnected()) tempIR = m_zmqObject->getValue(dCast(k)) / 65536;
            else tempIR = 0;
            painter->drawPie(QIRTriangle(m_coordinates, m_yaw + k*60, tempIR), (m_yaw + k*60 - 25)*16, 50*16); // 0° is at 3 o'clock, ccw direction
        }
    }

    //paint Temp sensor readings
    if(g_settings->value("temp_on").toBool()){
        for(int k = 0; k < 4; k++){
            if(m_zmqObject->isConnected()){
                double tempTemp = m_zmqObject->getValue(dCast(6 + k));
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

            QTempArc arc(m_coordinates, m_yaw + k*90); // 0° is at 3 o'clock, ccw direction
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
    if(isSelected() && m_inGroup){
        pen.setColor(m_groupColor);
    } else {
        if(m_zmqObject->isConnected()) {
            pen.setColor(Qt::green);
        } else {
            pen.setColor(Qt::red);
        }
    }

    if(isSelected()){
        pen.setStyle(Qt::DotLine);
    } else {
        pen.setStyle(Qt::SolidLine);
    }

    brush.setColor(m_zmqObject->getLedColor());
    if(tCast(m_treeItem)->isChildSelected()) brush.setStyle(Qt::Dense2Pattern);

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(boundingRect());
    painter->drawLine(m_coordinates.x() + 5*cos(-m_yaw*PI/180),
                      m_coordinates.y() + 5*sin(-m_yaw*PI/180),
                      m_coordinates.x() + 9*cos(-m_yaw*PI/180),
                      m_coordinates.y() + 9*sin(-m_yaw*PI/180));

    //paint airflow marker
    if(g_settings->value("air_on").toBool() && m_zmqObject->isConnected() && m_zmqObject->getState(Airflow)){
        double value = m_zmqObject->getValue(Airflow);

        pen.setColor(Qt::transparent);
        brush.setColor(QColor(250, 218, 94, 96));
        painter->setPen(pen);
        painter->setBrush(brush);
        // 30 FPS, max_speed = 6 deg/frame -> w = 0.5 rpm
        // CURRENTLY THERE IS ONLY ONE INTENSITY, WHEN INTESITY RANGE WILL BE ENABLED, MAX_SPEED SHOULD BE 12
        m_airflowAngle = fmod(m_airflowAngle + value * 6 * FPSrepaint, 360);
        painter->drawPath(QPetal(m_coordinates, m_airflowAngle));       // petal 1
        painter->drawPath(QPetal(m_coordinates, m_airflowAngle + 120)); // petal 2
        painter->drawPath(QPetal(m_coordinates, m_airflowAngle - 120)); // petal 3
    }

    //paint vibration marker
    if(g_settings->value("vibr_on").toBool() && m_zmqObject->isConnected() && m_zmqObject->getState(Speaker)){
        double freq = m_zmqObject->getValue(Frequency);
        double amplitude = m_zmqObject->getValue(Amplitude);

        pen.setColor(QColor(255,255,255,96));
        pen.setWidth(2);
        pen.setStyle(Qt::SolidLine);
        brush.setColor(Qt::transparent);
        painter->setPen(pen);
        painter->setBrush(brush);
        // 30 FPS, max_speed = 12 deg/frame -> w = 1 rpm
        m_vibrAngle = fmod(m_vibrAngle - amplitude/50* 12*FPSrepaint, 360);
        // wawesNum = [6 .. 15]
        int wawesNum = 6+9*freq/1500;
        auto tempItem = std::move(QVibratingCircle(m_coordinates, wawesNum, m_vibrAngle));
        painter->drawPath(tempItem);
        pen.setColor(QColor(128,128,128,96));
        painter->setPen(pen);
        painter->drawPoints(tempItem.points, 20);
    }

    if(FPSrepaint) FPScheck->start();
}

QIRTriangle::QIRTriangle(QPointF center, double angle, double value)
{
    double side = 5 + 32 * value; // Scale sensor reading pie to be ~18cm (~3x the edge of CASU ring) at maximum reading value (2^16)
    double offset = 3; // center offset from center of CASU

    angle = angle * PI/180;
    center += QPointF(offset*cos(angle), -offset*sin(angle));
    auto topLeft = center - QPointF(side*sqrt(2),side*sqrt(2));
    auto bottomRight = center + QPointF(side*sqrt(2),side*sqrt(2));

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
