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
    QCasuTreeItem(QCasuZMQ *zmqObject);
};

#endif // CASUTREEITEM_H
