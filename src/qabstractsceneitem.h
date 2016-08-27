#ifndef QABSTRACTSCENEITEM_H
#define QABSTRACTSCENEITEM_H

#include <QGraphicsItemGroup>
#include <QVariant>

#include "qcasuzmq.h"
#include "qabstracttreeitem.h"

class QAbstractSceneItem : public QGraphicsItemGroup
{
protected:
    bool _inGroup;
    QColor _groupColor;
    QTreeWidgetItem* _treeItem;

    //Protected virtual methods
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value){
        if(change == QGraphicsItem::ItemSelectedHasChanged){
            _treeItem->setHidden(!value.toBool());
            if(!value.toBool()) ((QAbstractTreeItem*) _treeItem)->resetSelection();
        }
        return QGraphicsItem::itemChange(change, value);
    }
public:
    QAbstractSceneItem(){}
    void setInGroup(bool state){
        _inGroup = state;
    }

    // Public pure virtual methods
    virtual bool isGroup() const = 0;
    virtual QList<zmqBuffer *> getBuffers(QCasuZMQ::dataType key) = 0;

    // Public virtual methods
    virtual void setGroupColor(QColor color){
        _groupColor = color;
    }
};

#endif // QABSTRACTSCENEITEM_H
