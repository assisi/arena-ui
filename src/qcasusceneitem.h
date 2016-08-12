#ifndef QCASUSCENEITEM_H
#define QCASUSCENEITEM_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>

#include "qcasutreeitem.h"

#define PI 3.14159265

/*!
 * \brief Graphics scene item for CASU
 *
 * All graphical elements for individual CASU are drawn with this item
 */

class QCasuSceneItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

private:
    int x_center;
    int y_center;
    int yaw_;

//ANIMATION
    /*!
     * \brief Timer that checks if graphics scene update was called with animation timer or some other event
     */
    QElapsedTimer* FPScheck;
    /*!
     * \brief Current angle of rotation animation
     *
     * Increment during scene update depends on sensor values
     */
    double airflowAngle;
    /*!
     * \brief Current angle of rotation animation
     *
     * Increment during scene update depends on sensor values
     */
    double vibrAngle;

public:
    /*!
     * \brief Pointer to corresponding QCasuTreeItem for same CASU
     */
    QCasuTreeItem* treeItem;

    QCasuSceneItem(QObject *parent, int x, int y, int yaw, QCasuTreeItem *widget);

    QRectF boundingRect() const;

    /*!
     * \brief Overloaded inherited function responsible for drawing this item
     *
     * All graphical elements are implemented in this
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected slots:

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
    QVibratingCircle(QPointF center, int waves, double angle);
};

#endif // QCASUSCENEITEM_H
