#ifndef ARENAUI_H
#define ARENAUI_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QProgressBar>
#include <QSplitter>
#include <QKeySequence>

#include <nzmqt/nzmqt.hpp>
#include <yaml-cpp/yaml.h>

#include "globalHeader.h"

#include "qarenascene.h"
#include "qdialogconnect.h"
#include "qdialogsettings.h"
#include "qdialogsetpoint.h"

#include "qcasuscenegroup.h"
#include "qcasusceneitem.h"
#include "qcasutreeitem.h"
#include "qcasutreegroup.h"

#include "qtrendplot.h"
#include "flowlayout.h"
#include "qdeploy.h"
#include "qgraphicsviewzoom.h"
//#include "qgstreamerview.h"



namespace Ui {
class ArenaUI;
}
/*!
 * \brief Main window class
 *
 */
class ArenaUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit ArenaUI(QWidget *parent = 0);
    ~ArenaUI();

private:
    /*!
     * \brief Pointer to main window
     */
    Ui::ArenaUI *ui;
    /*!
     * \brief Pointer to main graphics scene
     */
    QArenaScene *m_arenaScene;
    /*!
     * \brief YAML node to parse associated project files
     */
    YAML::Node m_assisiNode;

    /*!
     * \brief Tab in which trend plots are docked
     */
    QVBoxLayout* m_trendTab;

    //QGstreamerView* videoStream;

    /*!
     * \brief Timer responsible for 30FPS graphics scene refresh rate
     */
    QTimer* m_sceneUpdate;
    QSignalMapper* m_signalMapper;
    QList<QAction*> m_setpointActions;

    /*!
     * \brief Function which sorts arenaScene elements so overlaping items won't hide smaller items (rendering them unselectable)
     */
    void sortGraphicsScene();

    /*!
     * \brief Recursive function which saves grouping hieararchy of arenaScene items when saving a session
     */
    void groupSave(QSettings *saveState, const QList<QGraphicsItem *> &items, const QString &groupName);
    /*!
     * \brief Recursive function which loads grouping hieararchy of arenaScene items when loading a session
     */
    QList<QGraphicsItem *> groupLoad(YAML::Node *arenaNode, QSettings *loadState, const int &groupSize, QMap<QString, QCasuZMQ*>* linkMap, QProgressBar *progress);

private slots:
    /*!
     * \brief Action to load project file (*.assisi) or session file (*.arenaUI).
     *
     * If user loads project file with multiple layers, additional dialog will apear to choose one to display in UI.
     *
     * If user loads session file it will automaticaly select same layer as in saved session
     */
    void on_actionOpenArena_triggered();
    /*!
     * \brief Action to save session file (*.arenaUI)
     */
    void on_actionSave_triggered();
    /*!
     * \brief Action to group selected arenaScene items
     */
    void on_actionGroup_triggered();
    /*!
     * \brief Action to ungroup selected arenaScene items
     */
    void on_actionUngroup_triggered();
    /*!
     * \brief Action to open a dialog for configuring single CASU connection settings
     */
    void on_actionConnect_triggered();
    /*!
     * \brief Action to open a dialog for configurin program settings
     */
    void on_actionSettings_triggered();
    /*!
     * \brief Action to toggle Loggin
     */
    void on_actionToggleLog_triggered();
    /*!
     * \brief Action to plot selected QCasuTreeItem data in same trend on trendTab
     */
    void on_actionPlot_selected_in_same_trend_triggered();
    /*!
     * \brief Action to plot selected QCasuTreeItem data in private trends on trendTab
     */
    void on_actionPlot_selected_in_different_trends_triggered();
    /*!
     * \brief Action to toggle videoStream widget
     */
    void on_actionCamera_toggled(bool arg1);

    /*!
     * \brief Custom context menu dialog for arenaScene
     */
    void customContextMenu(const QPoint &pos);
};



/*!
 * \brief Mouse click handler for QArenaScene which handles item selecting
 *
 * Implemented common functionalities of selecting with Ctrl key and with mouse drag
 */
class MouseClickHandler : public QObject
{
    Q_OBJECT

public:
    MouseClickHandler(QGraphicsScene* scene, QObject* parent = 0);

protected:
    bool eventFilter(QObject* obj, QEvent* event);

private:
    QGraphicsScene* scene_;
    bool dragTrue;
    QList<QGraphicsItem*> selectedList;
};
// ------------------------------------------------------------------------
#endif // ARENAUI_H
