#ifndef QABSTRACTTREEITEM_H
#define QABSTRACTTREEITEM_H

#define tCast dynamic_cast<QAbstractTreeItem* >
#define tiCast dynamic_cast<QCasuTreeItem *>
#define tgCast dynamic_cast<QCasuTreeGroup *>

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

/*!
 * \brief Subclassed QTreeWidgetItem to disable sort on children
 */
class QNoSortTreeItem : public QTreeWidgetItem
{
public:
    template <typename ...Params>
    /*!
     * \brief Calls QTreeWigdetItem constructor with same parameters
     * \param params
     */
    QNoSortTreeItem(Params&&... params) : QTreeWidgetItem(forward<Params>(params)...) {}
    /*!
     * \brief reimplemented operator <
     * \param other [unused]
     * \return always false
     *
     * Always returns false so tree items will retain order in which they are inserted.
     */
    bool operator<(const QTreeWidgetItem& other) const {
        Q_UNUSED(other);
        return false;
    }
};

#endif // QABSTRACTTREEITEM_H
