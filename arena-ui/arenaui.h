#ifndef ARENAUI_H
#define ARENAUI_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QProgressBar>

#include <yaml-cpp/yaml.h>
#include <nzmqt/nzmqt.hpp>

#include "globalHeader.h"

#include "qcasusceneitem.h"
#include "qdialogconnect.h"
#include "qdialogsettings.h"
#include "qdialogsetpoint.h"
#include "qtrendplot.h"



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
    QString arenaFile;
    QVBoxLayout* trendTab;

    void groupSendSetpoint(QGraphicsItem *group, QList<QByteArray> message);

private slots:
    void on_actionOpen_Arena_triggered();
    void on_actionGroup_triggered();
    void on_actionUngroup_triggered();
    void on_actionConnect_triggered();
    void on_actionToggleLog_triggered();
    void on_actionPlot_selected_in_same_trend_triggered();
    void on_actionSettings_triggered();

    void customContextMenu(QPoint pos);

    void sendSetpoint(QString actuator);
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
};
// ------------------------------------------------------------------------
#endif // ARENAUI_H
