#ifndef QCASUTREEGROUP_H
#define QCASUTREEGROUP_H

#include "qabstracttreeitem.h"

class QCasuTreeGroup : public QAbstractTreeItem
{
private:
    QString _name;
public:
    explicit QCasuTreeGroup(QGraphicsItem* sceneItem);
};

class QSelectionTreeItem : public QCasuTreeGroup
{
private:
    QGraphicsScene *_scene;
public:
    explicit QSelectionTreeItem(QGraphicsScene *scene);
    QList<zmqBuffer *> getBuffers();
};
#endif // QCASUTREEGROUP_H
