#ifndef ARENAUI_H
#define ARENAUI_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <vector>

#include <yaml-cpp/yaml.h>
#include <nzmqt/nzmqt.hpp>

#include "casusceneitem.h"
#include "connectdialog.h"


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
    QString arena_file;

    YAML::Node arena_config;

private slots:
    void on_actionOpen_Arena_triggered();
    void on_actionGroup_triggered();
    void on_actionUngroup_triggered();
    void on_actionConnect_triggered();
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
