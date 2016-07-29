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
#include "qgstreamerview.h"



namespace Ui {
class ArenaUI;
}
// ------------------------------------------------------------------------
// Subclassed QGraphicsScene for a BUG [QTBUG-10138]
// http://www.qtcentre.org/threads/36953-QGraphicsItem-deselected-on-contextMenuEvent
class QArenaScene : public QGraphicsScene
{
    Q_OBJECT
public:
    QArenaScene(QWidget *parent);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};
// ------------------------------------------------------------------------
class ArenaUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit ArenaUI(QWidget *parent = 0);
    ~ArenaUI();

private:
    Ui::ArenaUI *ui;
    QArenaScene *arena_scene;

    YAML::Node assisiNode;

    QSplitter* sideLayout;
    QVBoxLayout* trendTab;
    QDeploy* deployWidget;
    QScrollArea* deployScroll;
    QLabel* deployArena;
    QLabel* deployFile;
    QLabel* deployNeighborhood;
    QGstreamerView* videoStream;

    void sortGraphicsScene();

    void groupSendSetpoint(QList<QGraphicsItem *> group, QList<QByteArray> message);
    void groupSave(QSettings *saveState, QList<QGraphicsItem*> items, QString groupName);
    QList<QGraphicsItem *> *groupLoad(YAML::Node *arenaNode, QSettings *loadState, int groupSize, QMap<QString, QCasuTreeItem *> *linker, QProgressBar *progress);

private slots:
    void on_actionOpen_Arena_triggered();
    void on_actionGroup_triggered();
    void on_actionUngroup_triggered();
    void on_actionConnect_triggered();
    void on_actionToggleLog_triggered();
    void on_actionPlot_selected_in_same_trend_triggered();
    void on_actionSettings_triggered();

    void toggleIR();
    void toggleTemp();
    void toggleAir();

    void updateTreeSelection();
    void customContextMenu(QPoint pos);
    void moveDeployScroll(int min, int max);

    void sendSetpoint(QString actuator);
    void on_actionSave_triggered();
    void on_actionPlot_selected_in_different_trends_triggered();
    void on_actionCamera_toggled(bool arg1);
};

// ------------------------------------------------------------------------

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
