#ifndef QABSTRACTTREEITEM_H
#define QABSTRACTTREEITEM_H

#include <QTreeWidgetItem>

#include "qcasuzmq.h"

class QAbstractSceneItem;
class QAbstractTreeItem : public QTreeWidgetItem
{
protected:
    QMap<dataType, QTreeWidgetItem*> _widgetMap;
    QGraphicsItem *_sceneItem;
public:
    QAbstractTreeItem();
    void resetSelection();
    void setSceneItem(QGraphicsItem *sceneItem);

    //Public virtual methods
    virtual bool isChildSelected();
    virtual QList<zmqBuffer *> getBuffers();
};

#endif // QABSTRACTTREEITEM_H
