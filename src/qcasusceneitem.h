#ifndef QCASUSCENEITEM_H
#define QCASUSCENEITEM_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>

#include "qabstractsceneitem.h"

#define PI 3.14159265

/*!
 * \brief Graphics scene item for CASU
 *
 * All graphical elements for individual CASU are drawn with this item
 */

class QCasuSceneItem : public QAbstractSceneItem
{
private:
    QPointF _coordinates;
    int _yaw;
    QElapsedTimer *FPScheck;

    double _airflowAngle;
    double _vibrAngle;

    QCasuZMQ *_zmqObject;

public:
    bool isGroup() const;
    QList<zmqBuffer *> getBuffers(dataType key);
    QVector<QPointF> getCoordinateVector();
    void sendSetpoint(QList<QByteArray> message);

    QCasuSceneItem(QPointF coordinates, double yaw, QCasuZMQ *zmqObject);

    QCasuZMQ *getZmqObject();

    QRectF boundingRect() const;

    /*!
     * \brief Overloaded inherited function responsible for drawing this item
     *
     * All graphical elements are implemented in this
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

/*!
 * \brief Calculates bounding rectangle for triangle dependant on IR sensor angle and value
 */
class QIRTriangle : public QRectF
{
public:
    QIRTriangle(QPointF center, double angle, double value);
};
/*!
 * \brief Calculates arc parameters for temperature sensors
 */
class QTempArc
{
public:
    QTempArc(QPointF center, double angle);
    int start;
    int span;
    QRectF rect;
};
/*!
 * \brief Calculates individual petal path for fan animation dependant on petal angle
 */
class QPetal : public QPainterPath
{
public:
    QPetal(QPointF center, double angle);
};
/*!
 * \brief Calculates vibrating circle path for vibration animation dependant on number of wawes and angle
 */
class QVibratingCircle : public QPainterPath
{
public:
    QPointF points[20];
    QVibratingCircle(QPointF center, int waves, double angle);
};

#endif // QCASUSCENEITEM_H
