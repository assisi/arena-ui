#ifndef QABSTRACTSCENEITEM_H
#define QABSTRACTSCENEITEM_H

#include <QGraphicsItemGroup>
#include <QVariant>

#include "qcasuzmq.h"

class QAbstractTreeItem;
class QAbstractSceneItem : public QGraphicsItemGroup
{
protected:
    bool _inGroup;
    QColor _groupColor;
    QTreeWidgetItem *_treeItem;

    //Protected virtual methods
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
public:
    QAbstractSceneItem(){
        this->setFlag(GraphicsItemFlag::ItemIsSelectable);
    }
    void setInGroup(bool state);
    void setTreeItem(QTreeWidgetItem *treeItem);

    // Public pure virtual methods
    virtual bool isGroup() const = 0;
    virtual QList<zmqBuffer *> getBuffers(dataType key) = 0;
    virtual QVector<QPointF> getCoordinateVector() = 0;
    virtual void sendSetpoint(QList<QByteArray> message) = 0;

    // Public virtual methods
    virtual void setGroupColor(QColor color);
};

#endif // QABSTRACTSCENEITEM_H
