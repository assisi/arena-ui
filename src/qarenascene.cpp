#include "qarenascene.h"
#include "qcasusceneitem.h"
#include "qcasutreegroup.h"

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
    if(!g_settings->value("temp_on").toBool()) return; //don't draw when not showing temp sensors

    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);

    auto m_view = this->views().first();
    double scale = m_view->transform().m11();

    QRectF model(m_view->mapToScene(QPoint(10,10)),QSizeF(180,15)/scale);

    QLinearGradient heatmap(m_view->mapToScene(QPoint(10,10)), m_view->mapToScene(QPoint(190,10)));
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

    painter->drawLine(m_view->mapToScene(QPoint(10,25)), m_view->mapToScene(QPoint(10,30)));
    painter->drawLine(m_view->mapToScene(QPoint(100,25)), m_view->mapToScene(QPoint(100,30)));
    painter->drawLine(m_view->mapToScene(QPoint(190,25)), m_view->mapToScene(QPoint(190,30)));

    auto font = painter->font();
    font.setPointSizeF(11 / scale);
    painter->setFont(font);
    painter->drawText(m_view->mapToScene(QPoint(11,36)), "20 °C");
    painter->drawText(m_view->mapToScene(QPoint(101,36)), "35 °C");
    painter->drawText(m_view->mapToScene(QPoint(191,36)), "50 °C");

    int time = 0;
    int connectedItems = 0;
    for(auto& item : items()){
        if(!sCast(item)->isGroup()){
            if(siCast(item)->getZmqObject()->isConnected()){
                time += siCast(item)->getZmqObject()->getAvgSamplingTime();
                connectedItems++;
            }
        }
    }
    if(connectedItems && g_settings->value("avgTime_on").toBool()){
        auto samplingTimePosition = m_view->rect().topRight() - QPoint(180,-20) - QPoint(20,0) * (m_view->verticalScrollBar()->isVisible()) ;
        auto tempText = QString("Avg. sample time: ") + QString::number(time/connectedItems) + QString("ms");
        painter->drawText(m_view->mapToScene(samplingTimePosition), tempText);
    }
}

QArenaScene::QArenaScene(QWidget *parent) : QGraphicsScene(parent)
{
    this->setItemIndexMethod(QGraphicsScene::NoIndex);

    connect(this, &QGraphicsScene::selectionChanged, [&](){
        auto tempList = this->selectedItems();
        if(tempList.size()>1) m_treeItem->setHidden(false);
        else m_treeItem->setHidden(true);

        int color = 14;

        for(auto& item : tempList){
            if(sCast(item)->isGroup()){
                sCast(item)->setGroupColor(static_cast<Qt::GlobalColor>(color++));
            }
        }
    });
}

void QArenaScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton) event->accept();
    else QGraphicsScene::mousePressEvent(event);
}

void QArenaScene::setTreeItem(QTreeWidgetItem *treeItem)
{
    m_treeItem = treeItem;
    m_treeItem->setHidden(true);
}
