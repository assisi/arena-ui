#ifndef QCASUSCENEGROUP_H
#define QCASUSCENEGROUP_H

#include <QGraphicsItemGroup>
#include <QPen>
#include <QPainter>

#include "qcasutreeitem.h"

class QCasuSceneGroup : public QGraphicsItemGroup
{

public:
    QColor groupColor;
    bool isTopLevel;
    QCasuTreeItem* treeItem;

    explicit QCasuSceneGroup(QGraphicsItem* parent = 0, QCasuTreeItem *widget = 0);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


signals:

public slots:

};

#endif // QCASUSCENEGROUP_H
