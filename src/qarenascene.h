#ifndef QARENASCENE_H
#define QARENASCENE_H

#include<QGraphicsScene>

class QTreeWidgetItem;

/*!
 * \brief Subclassed QGraphicsScene with overloaded \b mousePressEvent function as a bug workaround.
 *
 * [QTBUG-10138] - http://www.qtcentre.org/threads/36953-QGraphicsItem-deselected-on-contextMenuEvent
 */
class QArenaScene : public QGraphicsScene
{
    Q_OBJECT
private:
    QMetaObject::Connection m_lambda;
protected:
    void drawBackground(QPainter *painter, const QRectF &rect) Q_DECL_OVERRIDE;
    void drawForeground(QPainter *painter, const QRectF &rect) Q_DECL_OVERRIDE;
    QTreeWidgetItem *m_treeItem;
public:
    explicit QArenaScene(QWidget *parent);
    ~QArenaScene();
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void setTreeItem(QTreeWidgetItem *treeItem);
};

#endif // QARENASCENE_H
