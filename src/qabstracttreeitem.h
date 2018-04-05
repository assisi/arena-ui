#ifndef QABSTRACTTREEITEM_H
#define QABSTRACTTREEITEM_H

#define tCast(x) dynamic_cast<QAbstractTreeItem* >(x)
#define tiCast(x) dynamic_cast<QCasuTreeItem *>(x)
#define tgCast(x) dynamic_cast<QCasuTreeGroup *>(x)

#include <QTreeWidgetItem>

#include "qcasuzmq.h"

class QAbstractSceneItem;
class QAbstractTreeItem : public QTreeWidgetItem
{
protected:
    QMap<zmqData::dataType, QTreeWidgetItem*> m_widgetMap;
    QGraphicsItem *m_sceneItem;

    // only child classes can call constructor
    explicit QAbstractTreeItem() = default;

public:
    void resetSelection() const;
    void setSceneItem(QGraphicsItem *sceneItem);

    //Public virtual methods
    virtual bool isChildSelected() const;
    virtual QList<QSharedPointer<zmqData::zmqBuffer> > getBuffers() const;
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
    QNoSortTreeItem(Params&&... params) : QTreeWidgetItem(std::forward<Params>(params)...) {
        setTextAlignment(1, Qt::AlignRight);
        setFont(1, QFontDatabase::systemFont(QFontDatabase::FixedFont));
    }
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
