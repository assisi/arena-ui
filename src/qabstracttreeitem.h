#ifndef QABSTRACTTREEITEM_H
#define QABSTRACTTREEITEM_H

#include <QTreeWidgetItem>

#include "qcasuzmq.h"

class QAbstractTreeItem : public QTreeWidgetItem
{
protected:
    QMap<QCasuZMQ::dataType, QTreeWidgetItem*> _widgetMap;

public:
    QAbstractTreeItem(){}
    void resetSelection(){
        for(int k=0;k<this->childCount();k++)
            for(int i=0;i<this->child(k)->childCount();i++)
                this->child(k)->child(i)->setSelected(false);
    }

    //Public virtual methods
    virtual bool isChildSelected(){
        return false;
    }

};

#endif // QABSTRACTTREEITEM_H
