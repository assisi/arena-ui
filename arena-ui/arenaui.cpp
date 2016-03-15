#include <boost/lexical_cast.hpp>

#include <QThread>
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <typeinfo>

#include "arenaui.h"
#include "ui_arenaui.h"


using namespace nzmqt;
using namespace std;
using namespace AssisiMsg;
using namespace boost;


// -------------------------------------------------------------------------------

ArenaUI::ArenaUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArenaUI)
{
    loadConfig();
    ui->setupUi(this);
    ui->actionToggleLog->setChecked(log_on);

    //CASU TREE TAB
    ui->casuTree->addAction(ui->actionPlot_selected_in_same_trend);
    ui->casuTree->addAction(ui->actionPlot_selected_in_different_trends);

    //TREND TAB SCROLLABLE LAYOUT
    ui->tab_trend->setLayout(new QVBoxLayout);
    QWidget* tempWidget = new QWidget;
    QScrollArea* tempScroll = new QScrollArea;

    tempScroll->setWidget(tempWidget);
    tempScroll->setWidgetResizable(true);
    trendTab = new QVBoxLayout(tempWidget);

    ui->tab_trend->layout()->addWidget(tempScroll);

    //GRAPHICS SCENE
    arena_scene = new QGraphicsScene(this);
    arena_scene->setSceneRect(0,0,800,800);

    ui->arenaSpace->setScene(arena_scene);
    ui->arenaSpace->setDragMode(QGraphicsView::RubberBandDrag);

    MouseClickHandler* click_handler = new MouseClickHandler(arena_scene, this);
    arena_scene->installEventFilter(click_handler);

}

// -------------------------------------------------------------------------------

ArenaUI::~ArenaUI()
{
    saveConfig();
    delete ui;
}

// -------------------------------------------------------------------------------

MouseClickHandler::MouseClickHandler(QGraphicsScene* scene, QObject *parent) :
    QObject(parent),
    scene_(scene){}
// -------------------------------------------------------------------------------
bool MouseClickHandler::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::GraphicsSceneMouseMove){
        drag_true = true;
    }
    if (event->type() == QEvent::GraphicsSceneMouseRelease)
    {
        QGraphicsSceneMouseEvent* mouse_click = static_cast<QGraphicsSceneMouseEvent *>(event);

        QGraphicsItem * itemAtMouse= scene_->itemAt(mouse_click->scenePos().x(),mouse_click->scenePos().y(), QTransform());

        if (itemAtMouse && !drag_true)
            if(QApplication::keyboardModifiers() == Qt::CTRL){
                if(itemAtMouse->isSelected())itemAtMouse->setSelected(0);
                else itemAtMouse->setSelected(1);
            }
            else{
                scene_->clearSelection();
                itemAtMouse->setSelected(1);
            }
        else if(QApplication::keyboardModifiers() != Qt::CTRL && !drag_true)
            scene_->clearSelection();


        drag_true = false;

        return true;
    }
    else
    {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

// -------------------------------------------------------------------------------


void ArenaUI::on_actionOpen_Arena_triggered()
{
    arenaFile = QFileDialog::getOpenFileName(this,tr("Open Arena configuration file"), arenaFolder, tr("*.arena"));
    if(!arenaFile.toStdString().empty()){
        arenaFolder = arenaFile.left(arenaFile.lastIndexOf("/"));
        arena_scene->clear();
        ui->casuTree->clear();
            YAML::Node arena_config = YAML::LoadFile(arenaFile.toStdString());

        for(YAML::const_iterator it=arena_config["beearena"].begin(); it!=arena_config["beearena"].end(); it++){
            QString name = QString::fromStdString(it->first.as<std::string>());
            int xpos = arena_config["beearena"][name.toStdString()]["pose"]["x"].as<int>();
            int ypos = arena_config["beearena"][name.toStdString()]["pose"]["y"].as<int>();

            QCasuTreeItem* tempTree = new QCasuTreeItem(ui->casuTree, name);

            ui->casuTree->addTopLevelItem(tempTree);

            QCasuSceneItem* tempItem = new QCasuSceneItem(this, xpos*10+400, -ypos*10+400, tempTree);

            arena_scene->addItem(tempItem);

            tempTree->setAddr(QString::fromStdString(arena_config["beearena"][name.toStdString()]["sub_addr"].as<std::string>()),
                               QString::fromStdString(arena_config["beearena"][name.toStdString()]["pub_addr"].as<std::string>()),
                               QString::fromStdString(arena_config["beearena"][name.toStdString()]["msg_addr"].as<std::string>()));
        }

    }
}

void ArenaUI::on_actionGroup_triggered()
{
    QList<QGraphicsItem *> temp_itemList= arena_scene->selectedItems();
    arena_scene->clearSelection();
    QGraphicsItemGroup *temp_group = arena_scene->createItemGroup(temp_itemList);
    temp_group->setFlag(QGraphicsItem::ItemIsSelectable, true);
    temp_group->setFlag(QGraphicsItem::ItemIsFocusable, false);
    temp_group->setSelected(1);
    temp_group->setZValue(-1);
}


void ArenaUI::on_actionUngroup_triggered()
{
    QList<QGraphicsItem *> temp_itemList= arena_scene->selectedItems();
    for(int k=0;k<temp_itemList.size();k++)
        if(temp_itemList[k]->childItems().size()){
            QList<QGraphicsItem *> tempChildList = temp_itemList[k]->childItems();
            if(!temp_itemList[k]->childItems().size())temp_itemList[k]->setZValue(1);
            arena_scene->destroyItemGroup((QGraphicsItemGroup*)temp_itemList[k]);
            for(int i=0;i<tempChildList.size();i++){;
                tempChildList[i]->setSelected(0);
                tempChildList[i]->setSelected(1);
            }
        }
}

void ArenaUI::on_actionConnect_triggered()
{
    bool error = false;
    if (arena_scene->selectedItems().size() != 1) error = true; //Check if excactly one object is selected
    if (arena_scene->selectedItems().size() == 1)
        if(arena_scene->selectedItems()[0]->childItems().size()) error = true; // Check if object is single casu (no children)
    if(error){
        QMessageBox msgBox;
        msgBox.setWindowTitle("ERROR");
        msgBox.setText("Please select one CASU.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    QCasuSceneItem* temp = (QCasuSceneItem*)arena_scene->selectedItems()[0];
    QConnectDialog* addrDiag = new QConnectDialog(temp->widget_->sub_addr,temp->widget_->pub_addr,temp->widget_->msg_addr);
    if(addrDiag->exec()){
        temp->widget_->setAddr(addrDiag->sub_addr->text(), addrDiag->pub_addr->text(),addrDiag->msg_addr->text());
    }
}

void ArenaUI::on_actionToggleLog_triggered()
{
    ui->actionToggleLog->setChecked(log_on);
    QString question = QString("Are you sure you want to turn ") + (log_on ? QString("OFF") : QString("ON")) + QString(" logging?");
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Toggle Log", question , QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes)log_on = !log_on;
    ui->actionToggleLog->setChecked(log_on);
}

void ArenaUI::on_actionPlot_selected_in_same_trend_triggered()
{
    QTrendPlot* tempWidget = new QTrendPlot(ui->casuTree);
    trendTab->addWidget(tempWidget);

    tempWidget->addGraphList(ui->casuTree->selectedItems());
}
