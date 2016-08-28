#ifndef CASUTREEITEM_H
#define CASUTREEITEM_H

#include <QObject>

#include <qabstracttreeitem.h>


class QCasuTreeItem : public QObject, public QAbstractTreeItem
{
    Q_OBJECT
private:
    QCasuZMQ *_zmqObject;

public:
    QCasuTreeItem(QGraphicsItem* sceneItem, QCasuZMQ *zmqObject);

private slots:
    void updateData(dataType key);
};

#endif // CASUTREEITEM_H
