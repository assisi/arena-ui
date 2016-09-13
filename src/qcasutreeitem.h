#ifndef CASUTREEITEM_H
#define CASUTREEITEM_H

#include <QObject>

#include <qabstracttreeitem.h>


class QCasuTreeItem : public QObject, public QAbstractTreeItem
{
    Q_OBJECT
private:
    QCasuZMQ *m_zmqObject;
    QMetaObject::Connection m_zmqObjectConnection;

public:
    explicit QCasuTreeItem(QCasuZMQ *zmqObject);
    ~QCasuTreeItem();
};

#endif // CASUTREEITEM_H
