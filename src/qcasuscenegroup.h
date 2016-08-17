#ifndef QCASUSCENEGROUP_H
#define QCASUSCENEGROUP_H

#include <QGraphicsItemGroup>
#include <QPen>
#include <QPainter>

class QCasuSceneGroup : public QGraphicsItemGroup
{

public:
    QColor groupColor;
    bool isTopLevel;

    explicit QCasuSceneGroup(QGraphicsItem* parent = 0);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


signals:

public slots:

};

#endif // QCASUSCENEGROUP_H
