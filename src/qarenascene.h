#ifndef QARENASCENE_H
#define QARENASCENE_H

#include<QGraphicsScene>

#include"qcasusceneitem.h"
#include"qcasutreegroup.h"

/*!
 * \brief Subclassed QGraphicsScene with overloaded \b mousePressEvent function as a bug workaround.
 *
 * [QTBUG-10138] - http://www.qtcentre.org/threads/36953-QGraphicsItem-deselected-on-contextMenuEvent
 */
class QArenaScene : public QGraphicsScene
{
    Q_OBJECT
protected:
    void drawBackground(QPainter *painter, const QRectF &rect);
    void drawForeground(QPainter *painter, const QRectF &rect);
    QTreeWidgetItem *_treeItem;
public:
    QArenaScene(QWidget *parent);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void setTreeItem(QTreeWidgetItem *treeItem);
private slots:
    void checkSelection();
};

#endif // QARENASCENE_H
