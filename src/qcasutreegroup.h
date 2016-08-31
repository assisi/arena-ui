#ifndef QCASUTREEGROUP_H
#define QCASUTREEGROUP_H

#include "qabstracttreeitem.h"

class QCasuTreeGroup : public QAbstractTreeItem
{
protected:
    QString _name;
public:
    QCasuTreeGroup(QString name = QString("CASU group"));
};

class QSelectionTreeItem : public QCasuTreeGroup
{
private:
    QGraphicsScene *_scene;
public:
    QSelectionTreeItem(QGraphicsScene *scene);
    QList<zmqBuffer *> getBuffers() Q_DECL_OVERRIDE;
};
#endif // QCASUTREEGROUP_H
