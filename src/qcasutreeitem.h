#ifndef CASUTREEITEM_H
#define CASUTREEITEM_H

#include <QObject>

#include <qabstracttreeitem.h>


class QCasuTreeItem : public QObject, public QAbstractTreeItem
{
    Q_OBJECT
private:
    QCasuZMQ *_zmqObject;
    QMetaObject::Connection _zmqObjectConnection;

public:
    QCasuTreeItem(QCasuZMQ *zmqObject);
    ~QCasuTreeItem();
};

#endif // CASUTREEITEM_H
