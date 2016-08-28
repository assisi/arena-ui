#include "qarenascene.h"

// Subclassed QGraphicsScene for a BUG [QTBUG-10138]
// http://www.qtcentre.org/threads/36953-QGraphicsItem-deselected-on-contextMenuEvent
void QArenaScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);

    QPen pen;
    QBrush brush;
    pen.setColor(Qt::transparent);
    brush.setColor(Qt::lightGray);
    brush.setStyle(Qt::SolidPattern);

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(0,0,800,800);
}

void QArenaScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect)
    if(!settings->value("temp_on").toBool()) return; //don't draw when not showing temp sensors

    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);

    QGraphicsView* _view = this->views().first();
    double scale = _view->transform().m11();

    QRectF model(_view->mapToScene(QPoint(10,10)),QSizeF(180,15)/scale);

    QLinearGradient heatmap(_view->mapToScene(QPoint(10,10)),_view->mapToScene(QPoint(190,10)));
    heatmap.setColorAt(0, Qt::blue);
    heatmap.setColorAt(0.33, Qt::magenta);
    heatmap.setColorAt(0.66, Qt::red);
    heatmap.setColorAt(1, Qt::yellow);
    QPen pen(Qt::transparent);
    QBrush brush(heatmap);

    pen.setColor(Qt::transparent);

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRect(model);

    pen.setColor(Qt::black);
    pen.setWidth(0);
    painter->setPen(pen);

    painter->drawLine(_view->mapToScene(QPoint(10,25)),_view->mapToScene(QPoint(10,30)));
    painter->drawLine(_view->mapToScene(QPoint(100,25)),_view->mapToScene(QPoint(100,30)));
    painter->drawLine(_view->mapToScene(QPoint(190,25)),_view->mapToScene(QPoint(190,30)));

    QFont font = painter->font();
    font.setPointSizeF(11 / scale);
    painter->setFont(font);
    painter->drawText(_view->mapToScene(QPoint(11,36)), "20 °C");
    painter->drawText(_view->mapToScene(QPoint(101,36)), "35 °C");
    painter->drawText(_view->mapToScene(QPoint(191,36)), "50 °C");
}

QArenaScene::QArenaScene(QWidget *parent) : QGraphicsScene(parent){
    this->setItemIndexMethod(QGraphicsScene::NoIndex);
    connect(this,SIGNAL(selectionChanged()),SLOT(checkSelection()));
}

void QArenaScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton) event->accept();
    else QGraphicsScene::mousePressEvent(event);
}

void QArenaScene::checkSelection()
{
    QList<QGraphicsItem *> tempList = this->selectedItems();
    if(tempList.size()>1) selectionTreeWidget->setHidden(false);
    else selectionTreeWidget->setHidden(true);

    int color = 14;

    foreach(QGraphicsItem *item, tempList)
        if(dynamic_cast<QAbstractSceneItem *>(item)->isGroup())
            dynamic_cast<QAbstractSceneItem *>(item)->setGroupColor(static_cast<Qt::GlobalColor>(color++));
}
