#ifndef QABSTRACTSCENEITEM_H
#define QABSTRACTSCENEITEM_H

#define sCast(x) dynamic_cast<QAbstractSceneItem *>(x)
#define siCast(x) dynamic_cast<QCasuSceneItem *>(x)
#define sgCast(x) dynamic_cast<QCasuSceneGroup *>(x)


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

    // only child classes can call constructor
    explicit QAbstractSceneItem();

    //Protected virtual methods
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
public:
    void setInGroup(bool state);
    void setTreeItem(QTreeWidgetItem *treeItem);
    void deleteTreeItem();

    // Public pure virtual methods
    virtual bool isGroup() const = 0;
    virtual QList<zmqData::zmqBuffer *> getBuffers(zmqData::dataType key) const = 0;
    virtual QVector<QPointF> getCoordinateVector() const = 0;
    virtual void sendSetpoint(const QList<QByteArray> &message) const = 0;

    // Public virtual methods
    virtual void setGroupColor(const QColor &color);
    virtual QPainterPath shape() const;
    virtual QPainterPath completeShape() const;
};

#endif // QABSTRACTSCENEITEM_H
