#ifndef QTRENDPLOT_H
#define QTRENDPLOT_H

#include "qcustomplot.h"
#include "qtreebuffer.h"

class QTrendPlot : public QCustomPlot
{
    Q_OBJECT
public:
    explicit QTrendPlot(QWidget *parent = 0);
    ~QTrendPlot(){}

    void addCurve(QTreeBuffer *treeItem);
    void removeCurve();

protected:
    virtual int heightForWidth(int w) const {return w*9/16;}
    bool autoPosition;

signals:

public slots:
    void updatePlot(double time, double value);
    void enableAutoPosition();
    void disableAutoPosition(QMouseEvent *event);
    void setZoomFlags(QWheelEvent *event);
};

#endif // QTRENDPLOT_H
