#ifndef QABSTRACTSCENEITEM_H
#define QABSTRACTSCENEITEM_H

#include <memory>

#include <QGraphicsItemGroup>
#include <QVariant>

#include "qcasuzmq.h"

class QAbstractTreeItem;
class QAbstractSceneItem : public QGraphicsItemGroup
{
private:
    void recursiveSetHidden(bool state);
protected:
    bool _inGroup;
    QColor _groupColor;
    QTreeWidgetItem *_treeItem;

    //Protected virtual methods
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
public:
    QAbstractSceneItem();

    void setInGroup(bool state);
    void setTreeItem(QTreeWidgetItem *treeItem);
    void deleteTreeItem();

    // Public pure virtual methods
    virtual bool isGroup() const = 0;
    virtual QList<zmqBuffer *> getBuffers(dataType key) = 0;
    virtual QVector<QPointF> getCoordinateVector() = 0;
    virtual void sendSetpoint(QList<QByteArray> message) = 0;

    // Public virtual methods
    virtual void setGroupColor(QColor color);
    virtual QPainterPath shape() const;
};

#endif // QABSTRACTSCENEITEM_H
