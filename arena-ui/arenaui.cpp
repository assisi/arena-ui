#include <boost/lexical_cast.hpp>

#include <QThread>
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QMessageBox>

#include "global.h"
#include "arenaui.h"
#include "ui_arenaui.h"

#include "dev_msgs.pb.h"

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
    arena_file = QFileDialog::getOpenFileName(this,tr("Open Arena configuration file"), arena_folder, tr("*.arena"));
    if(!arena_file.toStdString().empty()){
        arena_scene->clear();
            YAML::Node arena_config = YAML::LoadFile(arena_file.toStdString());

        for(YAML::const_iterator it=arena_config["beearena"].begin(); it!=arena_config["beearena"].end(); it++){
            QString name = QString::fromStdString(it->first.as<std::string>());
            int xpos = arena_config["beearena"][name.toStdString()]["pose"]["x"].as<int>();
            int ypos = arena_config["beearena"][name.toStdString()]["pose"]["y"].as<int>();

            CasuTreeItem* temp_tree = new CasuTreeItem(ui->casuTree, name);

            ui->casuTree->addTopLevelItem(temp_tree);

            CasuSceneItem* temp_item = new CasuSceneItem(this, xpos*10+400, -ypos*10+400, temp_tree);

            arena_scene->addItem(temp_item);

            temp_tree->setAddr(QString::fromStdString(arena_config["beearena"][name.toStdString()]["sub_addr"].as<std::string>()),
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
            QList<QGraphicsItem *> temp_childList = temp_itemList[k]->childItems();
            if(!temp_itemList[k]->childItems().size())temp_itemList[k]->setZValue(1);
            arena_scene->destroyItemGroup((QGraphicsItemGroup*)temp_itemList[k]);
            for(int i=0;i<temp_childList.size();i++){;
                temp_childList[i]->setSelected(0);
                temp_childList[i]->setSelected(1);
            }
        }
}

void ArenaUI::on_actionConnect_triggered()
{
    //Check if only one is selected
    {
    bool error = false;
    if (arena_scene->selectedItems().size() != 1) error = true;
    if(arena_scene->selectedItems().size() == 1)
        if(arena_scene->selectedItems()[0]->childItems().size()) error = true;
    if(error){
        QMessageBox msgBox;
        msgBox.setWindowTitle("ERROR");
        msgBox.setText("Please select one CASU.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }}
    CasuSceneItem* temp = (CasuSceneItem*)arena_scene->selectedItems()[0];
    connectDialog* addrDiag = new connectDialog(temp->widget_->sub_addr,temp->widget_->pub_addr,temp->widget_->msg_addr);
    if(addrDiag->exec()){
        temp->widget_->setAddr(addrDiag->sub_addr->text(), addrDiag->pub_addr->text(),addrDiag->msg_addr->text());
    }
}
