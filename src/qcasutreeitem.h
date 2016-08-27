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
    bool isChildSelected();

private slots:
    void updateData(QCasuZMQ::dataType key);
};

#endif // CASUTREEITEM_H
