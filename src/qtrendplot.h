#ifndef QTRENDPLOT_H
#define QTRENDPLOT_H

#include "QCustomPlot/qcustomplot.h"
#include "qtreebuffer.h"

/*!
 * \brief Custom QCustomPlot class which defines behavior of trend plotting
 *
 *
 */
class QTrendPlot : public QCustomPlot
{
    Q_OBJECT
private:
    /*!
     * \brief Pointer to casuTree for reading selected QTreeBuffers
     */
    QTreeWidget* casuTree;
    bool autoPosition;
    bool showLegend;
    bool docked;

public:
    /*!
     * \brief Map which stores information about connected QTreeBuffer and QCPGraph items
     */
    QMap<QCPGraph*,QTreeBuffer*> connectionMap;

    explicit QTrendPlot(QTreeWidget* tree, QWidget *parent = 0);
    ~QTrendPlot(){}

    /*!
     * \brief Add single QCPGraph to this plot and connect it to corresponding QTreeBuffer
     * \param treeItem
     */
    void addGraph(QTreeBuffer *treeItem);
    /*!
     * \brief Parse multiple QTreeBuffer items
     * \param itemList
     */
    void addGraphList(QList<QTreeWidgetItem *> itemList);
    /*!
     * \brief Remove single QCPGraph and disconnect it from its source
     * \param graph
     */
    void removeGraph(QCPGraph* graph);

protected:
    /*!
     * \brief Overloaded function which is responsible for constat 9:16 size ratio
     * \param width
     * \return height
     */
    virtual int heightForWidth(int width) const {return width*9/16;}

signals:

public slots:
    /*!
     * \brief Enables auto positioning on double click event
     */
    void enableAutoPosition();
    /*!
     * \brief Disables auto positioning on mouse drag event
     */
    void disableAutoPosition(QMouseEvent *event);
    /*!
     * \brief Sets zoom flags (x and/or y axis) corresponding to pressed keys (Ctrl & Shift)
     */
    void setZoomFlags(QWheelEvent *event);
    /*!
     * \brief Synchronizes graph selecting in legend and on plot
     */
    void selectionChanged();
    /*!
     * \brief Shows QTrendPLot context menu
     * \param position
     */
    void showContextMenu(QPoint position);
    /*!
     * \brief Toggles legend visibility status
     */
    void toggleLegend();
    /*!
     * \brief Toggles between docked and undocked state
     */
    void dock_undock();
    /*!
     * \brief Context menu action for removing selected graphs
     */
    void removeSelectedGraphs();
    /*!
     * \brief Context menu action for adding selected graphs
     */
    void addSelectedGraphs();
    /*!
     * \brief Saves current plot to PDF in vectorized format
     */
    void saveToPDF();

private slots:
    /*!
     * \brief Called before plotting
     *
     * Calculates x-axis time labels angle so they are always readable
     *
     * Calculates view position if autoPosition is enabled so no show data goes out of bounds on y-axis and that x-axis moves accordingly with new received data
     */
    void prettyPlot();
};

#endif // QTRENDPLOT_H
