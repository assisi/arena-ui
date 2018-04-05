#ifndef QCASUTREEGROUP_H
#define QCASUTREEGROUP_H

#include "qabstracttreeitem.h"

class QCasuTreeGroup : public QAbstractTreeItem
{
protected:
    QString m_name;
public:
    QCasuTreeGroup(QString name = QString("CASU group"));
};

class QSelectionTreeItem : public QCasuTreeGroup
{
private:
    QGraphicsScene *m_scene;
public:
    explicit QSelectionTreeItem(QGraphicsScene *scene);
    QList<QSharedPointer <zmqData::zmqBuffer> > getBuffers() const Q_DECL_OVERRIDE;
};
#endif // QCASUTREEGROUP_H
