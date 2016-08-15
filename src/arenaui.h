#ifndef ARENAUI_H
#define ARENAUI_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QProgressBar>
#include <QSplitter>

#include <nzmqt/nzmqt.hpp>
#include <yaml-cpp/yaml.h>

#include "globalHeader.h"

//3rd party class implementations
#include "flowlayout.h"

#include "qdialogconnect.h"
#include "qdialogsettings.h"
#include "qdialogsetpoint.h"
#include "qcasusceneitem.h"
#include "qcolorbar.h"
#include "qtrendplot.h"
#include "qdeploy.h"
//#include "qgstreamerview.h"



namespace Ui {
class ArenaUI;
}

/*!
 * \brief Subclassed QGraphicsScene with overloaded \b mousePressEvent function as a bug workaround.
 *
 * BUG [QTBUG-10138] - http://www.qtcentre.org/threads/36953-QGraphicsItem-deselected-on-contextMenuEvent
 */
class QArenaScene : public QGraphicsScene
{
    Q_OBJECT
public:
    QArenaScene(QWidget *parent);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

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
    QArenaScene *arenaScene;
    /*!
     * \brief YAML node to parse associated project files
     */
    YAML::Node assisiNode;

    /*!
     * \brief Main widget which carries tabWidget and videoStream widget, and enables configurable size ratio between them
     */
    QSplitter* sideLayout;
    /*!
     * \brief Tab in which trend plots are docked
     */
    QVBoxLayout* trendTab;
    /*!
     * \brief Custom widget for deployment
     */
    QDeploy* deployWidget;
    /*!
     * \brief Pointer to shell output scroll area
     */
    QScrollArea* deployScroll;
    /*!
     * \brief Pointer to QLabel for showing loaded associated file
     */
    QLabel* deployArena;
    /*!
     * \brief Pointer to QLabel for showing loaded associated file
     */
    QLabel* deployFile;
    /*!
     * \brief Pointer to QLabel for showing loaded associated file
     */
    QLabel* deployNeighborhood;
    /*!
     * \brief Custom widget for video streaming
     */
    //QGstreamerView* videoStream;

    /*!
     * \brief Timer responsible for 30FPS graphics scene refresh rate
     */
    QTimer* sceneUpdate;

    /*!
     * \brief Function which sorts arenaScene elements so overlaping items won't hide smaller items (rendering them unselectable)
     */
    void sortGraphicsScene();

    /*!
     * \brief Recursive function to send setpoint to multiple and/or grouped CASUs
     */
    void groupSendSetpoint(QList<QGraphicsItem *> group, QList<QByteArray> message);
    /*!
     * \brief Recursive function which saves grouping hieararchy of arenaScene items when saving a session
     */
    void groupSave(QSettings *saveState, QList<QGraphicsItem*> items, QString groupName);
    /*!
     * \brief Recursive function which loads grouping hieararchy of arenaScene items when loading a session
     */
    QList<QGraphicsItem *> *groupLoad(YAML::Node *arenaNode, QSettings *loadState, int groupSize, QMap<QString, QCasuTreeItem *> *linker, QProgressBar *progress);

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
     * \brief Action for toggling QCasuSceneItem graphics elements
     */
    void toggleIR();
    /*!
     * \brief Action for toggling QCasuSceneItem graphics elements
     */
    void toggleTemp();
    /*!
     * \brief Action for toggling QCasuSceneItem graphics elements
     */
    void toggleAir();
    /*!
     * \brief Action for toggling QCasuSceneItem graphics elements
     */
    void toggleVibr();

    /*!
     * \brief Updates selected QCasuTreeItem elements in tabCasus
     */
    void updateTreeSelection();
    /*!
     * \brief Custom context menu dialog for arenaScene
     */
    void customContextMenu(QPoint pos);
    /*!
     * \brief Scrolls shell output window on tabDeploy according to updates
     */
    void moveDeployScroll(int min, int max);

    /*!
     * \brief Opens a dialog for configuring setpoint parameters of selected actuator and sends them to selected CASUs
     */
    void sendSetpoint(QString actuator);
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
    bool drag_true;
    QList<QGraphicsItem*> selectedList;
};
// ------------------------------------------------------------------------
#endif // ARENAUI_H
