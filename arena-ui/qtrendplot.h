#ifndef QTRENDPLOT_H
#define QTRENDPLOT_H

#include "qcustomplot.h"
#include "qtreebuffer.h"

class QTrendPlot : public QCustomPlot
{
    Q_OBJECT
private:
    QTreeWidget* casuTree;
    bool autoPosition;
    bool showLegend;
    bool docked;

public:
    QMap<QCPGraph*,QTreeBuffer*> connectionMap;

    explicit QTrendPlot(QTreeWidget* tree, QWidget *parent = 0);
    ~QTrendPlot(){}

    void addGraph(QTreeBuffer *treeItem);
    void addGraphList(QList<QTreeWidgetItem *> itemList);
    void removeGraph(QCPGraph* graph);

protected:
    virtual int heightForWidth(int w) const {return w*9/16;}

signals:

public slots:
    void updatePlot(double time, double value);
    void enableAutoPosition();
    void disableAutoPosition(QMouseEvent *event);
    void setZoomFlags(QWheelEvent *event);
    void selectionChanged();
    void showContextMenu(QPoint pos);
    void toggleLegend();
    void dock_undock();
    void removeSelectedGraphs();
    void addSelectedGraphs();
    void saveToPDF();
};

#endif // QTRENDPLOT_H
