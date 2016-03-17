#ifndef ARENAUI_H
#define ARENAUI_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <vector>

#include <yaml-cpp/yaml.h>
#include <nzmqt/nzmqt.hpp>

#include "globalHeader.h"

#include "qcasusceneitem.h"
#include "qdialogconnect.h"
#include "qdialogsettings.h"
#include "qtrendplot.h"



namespace Ui {
class ArenaUI;
}

class ArenaUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit ArenaUI(QWidget *parent = 0);
    ~ArenaUI();

private:
    Ui::ArenaUI *ui;
    QGraphicsScene *arena_scene;
    QString arenaFile;
    QVBoxLayout* trendTab;

private slots:
    void on_actionOpen_Arena_triggered();
    void on_actionGroup_triggered();
    void on_actionUngroup_triggered();
    void on_actionConnect_triggered();
    void on_actionToggleLog_triggered();
    void on_actionPlot_selected_in_same_trend_triggered();
    void on_actionSettings_triggered();
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
