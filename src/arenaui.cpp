#include <QThread>
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QMessageBox>

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
    QLocale::setDefault(QLocale::C); //override for localization to use C locale

    loadConfig();
    ui->setupUi(this);
    ui->actionToggleLog->setChecked(settings->value("log_on").toBool());

    //CASU TREE TAB
    ui->casuTree->addAction(ui->actionPlot_selected_in_same_trend);
    ui->casuTree->addAction(ui->actionPlot_selected_in_different_trends);
    ui->casuTree->header()->setSortIndicator(0,Qt::AscendingOrder);

    ui->groupTree->addAction(ui->actionPlot_selected_in_same_trend);
    ui->groupTree->addAction(ui->actionPlot_selected_in_different_trends);

    connect(ui->casuTree,SIGNAL(itemSelectionChanged()),this,SLOT(updateTreeSelection()));
    connect(ui->groupTree,SIGNAL(itemSelectionChanged()),this,SLOT(updateTreeSelection()));


    //TREND TAB SCROLLABLE LAYOUT

    QWidget* tempWidget = new QWidget;
    QScrollArea* tempScroll = new QScrollArea;

    tempScroll->setWidget(tempWidget);
    tempScroll->setWidgetResizable(true);
    _trendTab = new QVBoxLayout(tempWidget);

    ui->tabTrend->setLayout(new QVBoxLayout);
    ui->tabTrend->layout()->addWidget(tempScroll);

    //GRAPHICS SCENE
    _arenaScene = new QArenaScene(this);
    _arenaScene->setSceneRect(0,0,800,800);
    ui->arenaSpace->setScene(_arenaScene);
    ui->arenaSpace->setDragMode(QGraphicsView::RubberBandDrag);
    new QGraphicsViewZoom(ui->arenaSpace);


    MouseClickHandler* click_handler = new MouseClickHandler(_arenaScene, this);
    _arenaScene->installEventFilter(click_handler);

    connect(ui->arenaSpace,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));

    // - 30fps update

    _sceneUpdate = new QTimer(this);
    connect(_sceneUpdate, SIGNAL(timeout()),_arenaScene,SLOT(update()));
    _sceneUpdate->start(34);

    //DEPLOYMENT - TODO: remove all this excesive code and reimplement it
    ui->tabDeploy->setLayout(new QVBoxLayout);

    // - deployment header
    tempWidget = new QWidget;
    QGridLayout* tempLayout = new QGridLayout;
    _deployWidget = new QDeploy(this);

    _deployArena = new QLabel;
    _deployNeighborhood = new QLabel;
    _deployFile = new QLabel;

    QFont tempFont;
    tempFont.setBold(true);
    QLabel* tempLabel = new QLabel("Arena description file:");
    tempLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    tempLabel->setAlignment(Qt::AlignRight);
    tempLabel->setFont(tempFont);
    tempLayout->addWidget(tempLabel,1,1);
    tempLabel = new QLabel("Neighborhood file:");
    tempLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    tempLabel->setAlignment(Qt::AlignRight);
    tempLabel->setFont(tempFont);
    tempLayout->addWidget(tempLabel,2,1);
    tempLabel = new QLabel("Deployment file:");
    tempLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    tempLabel->setAlignment(Qt::AlignRight);
    tempLabel->setFont(tempFont);
    tempLayout->addWidget(tempLabel,3,1);

    tempLayout->addWidget(_deployArena,1,2);
    tempLayout->addWidget(_deployNeighborhood,2,2);
    tempLayout->addWidget(_deployFile,3,2);

    tempWidget->setLayout(tempLayout);
    ui->tabDeploy->layout()->addWidget(tempWidget);

    // - interaction buttons
    tempWidget = new QWidget;
    tempWidget->setLayout(new FlowLayout);

    QPushButton* tempButton = new QPushButton("Deploy");
    connect(tempButton,SIGNAL(clicked()), _deployWidget, SLOT(deploy()));
    tempWidget->layout()->addWidget(tempButton);

    tempButton = new QPushButton("Run");
    connect(tempButton,SIGNAL(clicked()), _deployWidget, SLOT(run()));
    tempWidget->layout()->addWidget(tempButton);

    tempButton = new QPushButton("Stop");
    connect(tempButton,SIGNAL(clicked()), _deployWidget, SLOT(stop()));
    tempWidget->layout()->addWidget(tempButton);

    tempButton = new QPushButton("Collect data");
    connect(tempButton,SIGNAL(clicked()), _deployWidget, SLOT(collect()));
    tempWidget->layout()->addWidget(tempButton);

    tempButton = new QPushButton("Clean log");
    connect(tempButton,SIGNAL(clicked()), _deployWidget, SLOT(cleanLog()));
    tempWidget->layout()->addWidget(tempButton);

    tempButton = new QPushButton("Start simulator");
    connect(tempButton,SIGNAL(clicked()), _deployWidget, SLOT(simulatorStart()));
    tempWidget->layout()->addWidget(tempButton);

    tempButton = new QPushButton("Stop simulator");
    connect(tempButton,SIGNAL(clicked()), _deployWidget, SLOT(simulatorStop()));
    tempWidget->layout()->addWidget(tempButton);

    ui->tabDeploy->layout()->addWidget(tempWidget);

    // - text output window
    _deployScroll = new QScrollArea;
    _deployScroll->setWidget(_deployWidget);
    _deployScroll->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    _deployScroll->setWidgetResizable(true);
    _deployScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->tabDeploy->layout()->addWidget(_deployScroll);

    connect(_deployScroll->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(moveDeployScroll(int,int)));

    tempWidget = new QWidget;
    tempWidget->setLayout(new FlowLayout);

    tempButton = new QPushButton("Clear output");
    tempButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    connect(tempButton,SIGNAL(clicked()), _deployWidget, SLOT(clear()));
    tempWidget->layout()->addWidget(tempButton);

    QCheckBox* temCheckBox = new QCheckBox("Catch shell output");
    temCheckBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    connect(temCheckBox,SIGNAL(stateChanged(int)), _deployWidget, SLOT(toggleOutput(int)));
    tempWidget->layout()->addWidget(temCheckBox);

    ui->tabDeploy->layout()->addWidget(tempWidget);
}

ArenaUI::~ArenaUI()
{
    delete ui;
}

void ArenaUI::sortGraphicsScene()
{
    QList<QGraphicsItem*> tempSelection = _arenaScene->selectedItems(); // save active selection
    QPainterPath pp;
    pp.addRect(_arenaScene->sceneRect()); // select all -- selectedItems() doesnt return group children which is a case with items()
    _arenaScene->setSelectionArea(pp);

    for(int k = 0; k < _arenaScene->selectedItems().size(); k++)_arenaScene->selectedItems()[k]->setZValue(k+1);

    for(int k = 0; k+1 < _arenaScene->selectedItems().size(); k++)
        for(int i = k+1; i < _arenaScene->selectedItems().size(); i++){
            QAbstractSceneItem *item1 = dynamic_cast<QAbstractSceneItem *>(_arenaScene->selectedItems()[k]);
            QAbstractSceneItem *item2 = dynamic_cast<QAbstractSceneItem *>(_arenaScene->selectedItems()[i]);

            QPainterPath path1 = item1->isGroup()? dynamic_cast<QCasuSceneGroup*>(item1)->completeShape() : item1->shape();
            QPainterPath path2 = item2->isGroup()? dynamic_cast<QCasuSceneGroup*>(item2)->completeShape() : item2->shape();

            if(path1.intersects(path2)){
                int z1 = item1->zValue();
                int z2 = item2->zValue();
                if(z1 > z2) swap(z1,z2);

                if(path1.intersects(item2->shape())){
                    item1->setZValue(z1);
                    item2->setZValue(z2);
                }
                if(path2.intersects(item1->shape())){
                    item1->setZValue(z2);
                    item2->setZValue(z1);
                }
            }
        }

    pp = pp.subtracted(pp);
    pp.addRect(0,0,0,0);
    _arenaScene->setSelectionArea(pp);
    foreach(QGraphicsItem* item, tempSelection) item->setSelected(true); // after saving items and groups, return selection as was before

}

// -------------------------------------------------------------------------------

MouseClickHandler::MouseClickHandler(QGraphicsScene* scene, QObject *parent) :
    QObject(parent),
    scene_(scene){}

bool MouseClickHandler::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::GraphicsSceneMousePress){
        if(((QGraphicsSceneMouseEvent*)event)->button() == Qt::RightButton) return QObject::eventFilter(obj, event);
        if(QApplication::keyboardModifiers() == Qt::ControlModifier)
            selectedList = scene_->selectedItems();
        else selectedList.clear();

        return true;
    }
    else
    if (event->type() == QEvent::GraphicsSceneMouseMove){
        foreach(QGraphicsItem* item, selectedList) item->setSelected(true);
        drag_true = true;
        scene_->update();

        return true;
    }
    else
    if (event->type() == QEvent::GraphicsSceneMouseRelease)
    {
        foreach(QGraphicsItem* item, selectedList) item->setSelected(true);

        QGraphicsSceneMouseEvent* mouse_click = static_cast<QGraphicsSceneMouseEvent *>(event);
        QGraphicsItem * itemAtMouse= scene_->itemAt(mouse_click->scenePos().x(),mouse_click->scenePos().y(), QTransform());

        if (itemAtMouse && !drag_true)
            if(QApplication::keyboardModifiers() == Qt::ControlModifier){
                if(itemAtMouse->isSelected())itemAtMouse->setSelected(0);
                else itemAtMouse->setSelected(1);
            }
            else{
                scene_->clearSelection();
                itemAtMouse->setSelected(1);
            }
        else if(QApplication::keyboardModifiers() != Qt::ControlModifier && !drag_true)
            scene_->clearSelection();


        drag_true = false;
        scene_->update();

        return true;
    }
    else
    {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

// -------------------------------------------------------------------------------

void ArenaUI::on_actionOpenArena_triggered()
{
    QProgressBar progress;
    progress.setMinimum(0);
    QString loadFile = QFileDialog::getOpenFileName(this,tr("Open Arena configuration file"), settings->value("arenaFolder").toString(), tr("All(*.arenaUI *assisi);;Project(*.assisi);;Session(*.arenaUI)"));
    if(!loadFile.size()) return;

    while(_trendTab->count()){
        ((QCustomPlot*)_trendTab->itemAt(0)->widget())->close();
        _trendTab->removeWidget(_trendTab->itemAt(0)->widget());
    }

    _arenaScene->clear();
    ui->casuTree->clear();
    ui->groupTree->clear();
    /*Create new selectionTreeItem*/{
        QSelectionTreeItem *tempTreeItem = new QSelectionTreeItem(_arenaScene);
        ui->groupTree->addTopLevelItem(tempTreeItem);
        _arenaScene->setTreeItem(tempTreeItem);
    }

// FIXME: github issue #39
    if(loadFile.endsWith(".assisi")){
        assisiFile.name = loadFile;
        _assisiNode = YAML::LoadFile(assisiFile.name.toStdString());

        _deployArena->setText(QString::fromStdString(_assisiNode["arena"].as<std::string>()));
        _deployFile->setText(QString::fromStdString(_assisiNode["dep"].as<std::string>()));
        _deployNeighborhood->setText(QString::fromStdString(_assisiNode["nbg"].as<std::string>()));

        _deployWidget->setWorkingDirectory(assisiFile.name.left(assisiFile.name.lastIndexOf('/')));

        assisiFile.arenaFile = assisiFile.name.left(assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(_assisiNode["arena"].as<std::string>());
        assisiFile.depFile = assisiFile.name.left(assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(_assisiNode["dep"].as<std::string>());
        assisiFile.ngbFile = assisiFile.name.left(assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(_assisiNode["nbg"].as<std::string>());

        YAML::Node arenaNode = YAML::LoadFile(assisiFile.arenaFile.toStdString());
        QList<QString> layers;
        for(YAML::const_iterator it=arenaNode.begin(); it!=arenaNode.end(); it++) layers.append(QString::fromStdString(it->first.as<std::string>()));

        if(layers.size() > 1) assisiFile.arenaLayer = QInputDialog::getItem(this,tr("Select arena layer"),"",QStringList(layers));
        else assisiFile.arenaLayer = layers[0];

        progress.setMaximum(arenaNode[assisiFile.arenaLayer.toStdString()].size());
        progress.show();
        progress.move(ui->arenaSpace->mapToGlobal(QPoint(400-progress.width()/2,400-progress.height()/2)));

        for(YAML::const_iterator it=arenaNode[assisiFile.arenaLayer.toStdString()].begin(); it!=arenaNode[assisiFile.arenaLayer.toStdString()].end(); it++){
            QString name = QString::fromStdString(it->first.as<std::string>());
            QPointF coordinates;
            coordinates.setX(400 + 10 * arenaNode[assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["x"].as<double>());
            coordinates.setY(400 - 10 * arenaNode[assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["y"].as<double>());
            double yaw = arenaNode[assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["yaw"].as<double>();

            QCasuZMQ *tempZMQ = new QCasuZMQ(this, name);
            QCasuTreeItem *tempTreeItem = new QCasuTreeItem(tempZMQ);
            QCasuSceneItem *tempSceneItem = new QCasuSceneItem(coordinates, yaw, tempZMQ);

            tempTreeItem->setSceneItem(tempSceneItem);
            tempSceneItem->setTreeItem(tempTreeItem);

            ui->casuTree->addTopLevelItem(tempTreeItem);
            _arenaScene->addItem(tempSceneItem);
            tempTreeItem->setHidden(true);

            tempZMQ->setAddresses(QString::fromStdString(arenaNode[assisiFile.arenaLayer.toStdString()][name.toStdString()]["sub_addr"].as<std::string>()),
                               QString::fromStdString(arenaNode[assisiFile.arenaLayer.toStdString()][name.toStdString()]["pub_addr"].as<std::string>()),
                               QString::fromStdString(arenaNode[assisiFile.arenaLayer.toStdString()][name.toStdString()]["msg_addr"].as<std::string>()));

            progress.setValue(progress.value()+1);
            QApplication::processEvents();
        }
    }
    else if(loadFile.endsWith(".arenaUI")){

        QSettings loadSession(loadFile,QSettings::IniFormat);

        //GENERAL INFORMATION
        assisiFile.name = loadSession.value("assisiFile").toString();
        assisiFile.arenaLayer = loadSession.value("arenaLayer").toString();

        _assisiNode = YAML::LoadFile(assisiFile.name.toStdString());

        _deployArena->setText(QString::fromStdString(_assisiNode["arena"].as<std::string>()));
        _deployFile->setText(QString::fromStdString(_assisiNode["dep"].as<std::string>()));
        _deployNeighborhood->setText(QString::fromStdString(_assisiNode["nbg"].as<std::string>()));

        _deployWidget->setWorkingDirectory(assisiFile.name.left(assisiFile.name.lastIndexOf('/')));

        assisiFile.arenaFile = assisiFile.name.left(assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(_assisiNode["arena"].as<std::string>());
        assisiFile.depFile = assisiFile.name.left(assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(_assisiNode["dep"].as<std::string>());
        assisiFile.ngbFile = assisiFile.name.left(assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(_assisiNode["nbg"].as<std::string>());

        YAML::Node arenaNode = YAML::LoadFile(assisiFile.arenaFile.toStdString());
        QMap<QString, QCasuZMQ *> linker;

        //LOAD AND SPAWN CASUs
        loadSession.beginGroup("sceneHierarchy");
        int tempSize = loadSession.beginReadArray("main");
        progress.setMaximum(progress.maximum() + tempSize);
        groupLoad(&arenaNode, &loadSession, tempSize, &linker, &progress);
        loadSession.endArray();
        loadSession.endGroup();
        sortGraphicsScene();

        //LOAD AND SET CONNECTIONS
        loadSession.beginGroup("connectionSettings");
        tempSize = loadSession.beginReadArray("addresses");
        for(int k = 0; k < tempSize; k++){
            loadSession.setArrayIndex(k);
            linker[loadSession.value("casuName").toString()]->setAddresses(loadSession.value("sub_addr").toString(),
                                                                      loadSession.value("pub_addr").toString(),
                                                                      loadSession.value("msg_addr").toString());
        }
        loadSession.endArray();
        loadSession.endGroup();

        //LOAD AND LINK TREND GRAPHS
        loadSession.beginGroup("trendGraphs");
        tempSize = loadSession.beginReadArray("plot");
        for(int k = 0; k < tempSize; k++){
            loadSession.setArrayIndex(k);
            int graphSize = loadSession.beginReadArray("graph");
            QList<zmqBuffer *> toAdd;

            for(int i = 0; i < graphSize; i++){
                loadSession.setArrayIndex(i);
                QString casuName = loadSession.value("casuName").toString();
                dataType key = static_cast<dataType>(loadSession.value("key").toInt());
                toAdd.append(linker[casuName]->getBuffer(key));
            }
            if(toAdd.size()){
                QTrendPlot* tempWidget = new QTrendPlot(ui->casuTree, ui->groupTree);
                _trendTab->addWidget(tempWidget);
                tempWidget->addGraphList(toAdd);
                tempWidget->setWindowTitle(assisiFile.arenaLayer);
            }

            loadSession.endArray();
        }
        loadSession.endArray();
        loadSession.endGroup();
    }

    this->setWindowTitle("ASSISI - " + loadFile.mid(loadFile.lastIndexOf("/")) + ": " + assisiFile.arenaLayer);
}

void ArenaUI::on_actionGroup_triggered()
{

    QList<QGraphicsItem *> itemList= _arenaScene->selectedItems();
    if(itemList.size()<2) return;
    _arenaScene->clearSelection();

    QCasuTreeGroup* tempTreeGroup = new QCasuTreeGroup();
    QCasuSceneGroup* tempSceneGroup = new QCasuSceneGroup();
    tempTreeGroup->setSceneItem(tempSceneGroup);
    tempSceneGroup->setTreeItem(tempTreeGroup);

    tempSceneGroup->addToGroup(itemList);

    ui->groupTree->addTopLevelItem(tempTreeGroup);
    _arenaScene->addItem(tempSceneGroup);
    tempSceneGroup->setSelected(true);

    this->sortGraphicsScene();
}


void ArenaUI::on_actionUngroup_triggered()
{
    QList<QGraphicsItem *> itemList= _arenaScene->selectedItems();
    foreach(QGraphicsItem* item, itemList)
        if(dynamic_cast<QAbstractSceneItem *>(item)->isGroup()){
            dynamic_cast<QCasuSceneGroup *>(item)->removeFromGroup(item->childItems());
            dynamic_cast<QAbstractSceneItem *>(item)->deleteTreeItem();
            _arenaScene->destroyItemGroup(dynamic_cast<QAbstractSceneItem *>(item));
        }
    this->sortGraphicsScene();
}

void ArenaUI::on_actionConnect_triggered()
{
    bool error = false;
    if (_arenaScene->selectedItems().size() != 1) error = true; //Check if excactly one object is selected
    if(dynamic_cast<QAbstractSceneItem *>(_arenaScene->selectedItems().first())->isGroup()) error = true; // Check if object is single casu (no children)
    if(error){
        QMessageBox msgBox;
        msgBox.setWindowTitle("ERROR");
        msgBox.setText("Please select one CASU.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    QCasuSceneItem* temp = (QCasuSceneItem*)_arenaScene->selectedItems()[0];
    QDialogConnect* addrDiag = new QDialogConnect(temp->getAddresses());
    if(addrDiag->exec()){
        temp->setAddresses(addrDiag->getAddresses());
    }
}

void ArenaUI::on_actionToggleLog_triggered()
{
    ui->actionToggleLog->setChecked(settings->value("log_on").toBool());
    QString question = QString("Are you sure you want to turn ") + (settings->value("log_on").toBool() ? QString("OFF") : QString("ON")) + QString(" logging?");
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Toggle Log", question , QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes)settings->setValue("log_on", !settings->value("log_on").toBool());
    ui->actionToggleLog->setChecked(settings->value("log_on").toBool());
}

void ArenaUI::on_actionPlot_selected_in_same_trend_triggered()
{
    QTrendPlot* tempWidget = new QTrendPlot(ui->casuTree, ui->groupTree);
    _trendTab->addWidget(tempWidget);

    tempWidget->addSelectedGraphs();
    tempWidget->setWindowTitle(assisiFile.arenaLayer);
}

void ArenaUI::on_actionPlot_selected_in_different_trends_triggered()
{
    QList<zmqBuffer *> bufferList;

    for(int k=0; k < ui->casuTree->topLevelItemCount(); k++)
        bufferList.append(dynamic_cast<QAbstractTreeItem *>(ui->casuTree->topLevelItem(k))->getBuffers());
    for(int k=0; k < ui->groupTree->topLevelItemCount(); k++)
        bufferList.append(dynamic_cast<QAbstractTreeItem *>(ui->groupTree->topLevelItem(k))->getBuffers());

    foreach(zmqBuffer* buffer, bufferList){
        QList<zmqBuffer*> tempList;
        tempList.append(buffer);

        QTrendPlot* tempWidget = new QTrendPlot(ui->casuTree, ui->groupTree);
        _trendTab->addWidget(tempWidget);

        tempWidget->addGraphList(tempList);
        tempWidget->setWindowTitle(assisiFile.arenaLayer);
    }
}

void ArenaUI::on_actionSettings_triggered()
{
    QDialogSettings* settingsDiag = new QDialogSettings();
    settingsDiag->exec();
}

void ArenaUI::toggleIR()
{
    settings->setValue("IR_on",!settings->value("IR_on").toBool());
}

void ArenaUI::toggleTemp()
{
    settings->setValue("temp_on",!settings->value("temp_on").toBool());
}

void ArenaUI::toggleAir()
{
    settings->setValue("air_on",!settings->value("air_on").toBool());
}

void ArenaUI::toggleVibr()
{
    settings->setValue("vibr_on",!settings->value("vibr_on").toBool());
}

void ArenaUI::toggleAvgTime()
{
    settings->setValue("avgTime_on",!settings->value("avgTime_on").toBool());
}

void ArenaUI::updateTreeSelection()
{
    if(ui->casuTree->selectedItems().size() || ui->groupTree->selectedItems().size()){
        ui->actionPlot_selected_in_same_trend->setEnabled(true);
        ui->actionPlot_selected_in_different_trends->setEnabled(true);
    }
    else{
        ui->actionPlot_selected_in_same_trend->setEnabled(false);
        ui->actionPlot_selected_in_different_trends->setEnabled(false);
    }
}

void ArenaUI::customContextMenu(QPoint pos)
{
    QMenu* menu = new QMenu(); //no parent because it inherits background image from QGraphicsView
    QAction* temp;

    menu->setAttribute(Qt::WA_DeleteOnClose);

    bool error_single = false;
    if (_arenaScene->selectedItems().size() != 1) error_single = true; //Check if excactly one object is selected
    if (_arenaScene->selectedItems().size() == 1)
        if(_arenaScene->selectedItems()[0]->childItems().size()) error_single = true; // Check if object is single casu (no children)

    bool error_selected = !_arenaScene->selectedItems().size();

    menu->addAction(settings->value("IR_on").toBool() ? "Hide proximity sensors" : "Show proximity sensors",this,SLOT(toggleIR()));
    menu->addAction(settings->value("temp_on").toBool() ? "Hide temperature sensors" : "Show temperature sensors",this,SLOT(toggleTemp()));
    menu->addAction(settings->value("vibr_on").toBool() ? "Hide vibration marker" : "Show vibration marker",this,SLOT(toggleVibr()));
    menu->addAction(settings->value("air_on").toBool() ? "Hide airflow marker" : "Show airflow marker",this,SLOT(toggleAir()));
    menu->addAction(settings->value("avgTime_on").toBool() ? "Hide avg. sample time" : "Show avg. sample time",this,SLOT(toggleAvgTime()));
    menu->addSeparator();
    temp = menu->addAction("Group selected",this,SLOT(on_actionGroup_triggered()));
    if(error_selected) temp->setEnabled(false);
    temp = menu->addAction("Ungroup selected",this,SLOT(on_actionUngroup_triggered()));
    if(error_selected) temp->setEnabled(false);
    menu->addSeparator();
    temp = menu->addAction("Set connection",this,SLOT(on_actionConnect_triggered()));
    if(error_single) temp->setEnabled(false);

    QMenu* sendMenu = new QMenu("Setpoint");
    if(error_selected) sendMenu->setEnabled(false);
    menu->addMenu(sendMenu);

    QSignalMapper* signalMapper = new QSignalMapper(menu);

    temp = sendMenu->addAction("Temperature",signalMapper,SLOT(map()));
    signalMapper->setMapping(temp,"Temperature");
    temp = sendMenu->addAction("Vibration",signalMapper,SLOT(map()));
    signalMapper->setMapping(temp,"Vibration");
    temp = sendMenu->addAction("Airflow",signalMapper,SLOT(map()));
    signalMapper->setMapping(temp,"Airflow");
    temp = sendMenu->addAction("LED",signalMapper,SLOT(map()));
    signalMapper->setMapping(temp,"LED");
    temp = sendMenu->addAction("IR Proximity",signalMapper,SLOT(map()));
    signalMapper->setMapping(temp,"IR Proximity");

    connect(signalMapper,SIGNAL(mapped(QString)),this,SLOT(sendSetpoint(QString)));

    menu->popup(ui->arenaSpace->mapToGlobal(pos));
}

void ArenaUI::moveDeployScroll(int min, int max)
{
    Q_UNUSED(min)
    _deployScroll->verticalScrollBar()->setValue(max);
}

void ArenaUI::sendSetpoint(QString actuator)
{
    QDialogSetpoint* dialog = new QDialogSetpoint(actuator,_arenaScene->selectedItems());
    if(dialog->exec())
        foreach(QGraphicsItem* item, _arenaScene->selectedItems())
            dynamic_cast<QAbstractSceneItem *>(item)->sendSetpoint(dialog->getMessage());
}

void ArenaUI::groupSave(QSettings *saveState, QList<QGraphicsItem *> group, QString groupName)
{
    saveState->beginWriteArray(groupName);
    for(int k=0; k < group.size() ; k++){
        saveState->setArrayIndex(k);
        if(!dynamic_cast<QAbstractSceneItem *>(group[k])->isGroup()) saveState->setValue("casuName",dynamic_cast<QCasuSceneItem *>(group[k])->getName());
        else groupSave(saveState, group[k]->childItems(), "group");
    }
    saveState->endArray();
}

QList<QGraphicsItem *>* ArenaUI::groupLoad(YAML::Node* arenaNode, QSettings *loadState, int groupSize, QMap<QString, QCasuZMQ *> *linkMap, QProgressBar* progress)
{
    QList<QGraphicsItem *>* returnGroup = new QList<QGraphicsItem *>;
    for(int k=0; k < groupSize; k++){
        loadState->setArrayIndex(k);
        progress->setValue(progress->value()+1);

        if(loadState->childKeys().size()){
            QString name = loadState->value("casuName").toString();
            QPointF coordinates;
            coordinates.setX(400 + 10 * (*arenaNode)[assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["x"].as<double>());
            coordinates.setY(400 - 10 * (*arenaNode)[assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["y"].as<double>());
            double yaw = (*arenaNode)[assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["yaw"].as<double>();

            QCasuZMQ *tempZMQ = new QCasuZMQ(this, name);
            QCasuTreeItem *tempTreeItem = new QCasuTreeItem(tempZMQ);
            QCasuSceneItem *tempSceneItem = new QCasuSceneItem(coordinates, yaw, tempZMQ);

            tempTreeItem->setSceneItem(tempSceneItem);
            tempSceneItem->setTreeItem(tempTreeItem);

            ui->casuTree->addTopLevelItem(tempTreeItem);
            _arenaScene->addItem(tempSceneItem);
            tempTreeItem->setHidden(true);

            linkMap->insert(name, tempZMQ);

            progress->setValue(progress->value()+1);
            returnGroup->append(tempSceneItem);
        }
        else{
            int tempSize = loadState->beginReadArray("group");
            progress->setMaximum(progress->maximum() + tempSize);

            QCasuTreeGroup* tempTreeGroup = new QCasuTreeGroup();
            QCasuSceneGroup* tempSceneGroup = new QCasuSceneGroup();
            tempTreeGroup->setSceneItem(tempSceneGroup);
            tempSceneGroup->setTreeItem(tempTreeGroup);

            QList<QGraphicsItem *> itemList= *groupLoad(arenaNode, loadState, tempSize, linkMap, progress);
            tempSceneGroup->addToGroup(itemList);

            ui->groupTree->addTopLevelItem(tempTreeGroup);
            _arenaScene->addItem(tempSceneGroup);

            tempTreeGroup->setSelected(0);
            loadState->endArray();
            returnGroup->append(tempSceneGroup);
        }

        QApplication::processEvents();
    }
   return returnGroup;
}

void ArenaUI::on_actionSave_triggered()
{
    QString saveFile = QFileDialog::getSaveFileName(this,tr("Save As"),settings->value("arenaFolder").toString(),tr("*.arenaUI"));

    if(!saveFile.size())return;
    if(!saveFile.endsWith(".arenaUI")) saveFile += ".arenaUI";
    if(QFile(saveFile).exists())QFile(saveFile).remove();

    QSettings saveState(saveFile,QSettings::IniFormat);

    saveState.setValue("assisiFile",assisiFile.name);
    saveState.setValue("arenaLayer",assisiFile.arenaLayer);

    //save CASU graphics scene

    QList<QGraphicsItem*> tempSelection = _arenaScene->selectedItems(); // save active selection
    QPainterPath pp;
    pp.addRect(_arenaScene->sceneRect()); // select all -- selectedItems() doesnt return group children which is a case with items()
    _arenaScene->setSelectionArea(pp);

    saveState.beginGroup("sceneHierarchy");
    groupSave(&saveState, _arenaScene->selectedItems(),"main");
    saveState.endGroup();

    pp = pp.subtracted(pp);
    pp.addRect(0,0,0,0);
    _arenaScene->setSelectionArea(pp);
    foreach(QGraphicsItem* item, tempSelection) item->setSelected(true); // after saving items and groups, return selection as was before

    //save trend position and graphs

    saveState.beginGroup("trendGraphs");
    saveState.beginWriteArray("plot");
    for(int k=0; k < _trendTab->count() ; k++){
        QTrendPlot* tempPlot = (QTrendPlot*) _trendTab->itemAt(k)->widget();
        saveState.setArrayIndex(k);
        saveState.beginWriteArray("graph");
        for(int i=0; i < tempPlot->graphCount(); i++){
            saveState.setArrayIndex(i);
            saveState.setValue("casuName",tempPlot->link(tempPlot->graph(i))->getCasuName());
            saveState.setValue("key",static_cast<int>(tempPlot->link(tempPlot->graph(i))->getDataType()));
        }
        saveState.endArray();
    }
    saveState.endArray();
    saveState.endGroup();

    //save connection settins

    saveState.beginGroup("connectionSettings");
    saveState.beginWriteArray("addresses");
    int index = 0;
    foreach(QGraphicsItem *item, _arenaScene->items())
        if(!dynamic_cast<QAbstractSceneItem *>(item)->isGroup()){
            saveState.setArrayIndex(index++);
            saveState.setValue("casuName",dynamic_cast<QCasuSceneItem *>(item)->getName());
            QStringList addresses = dynamic_cast<QCasuSceneItem *>(item)->getAddresses();
            saveState.setValue("sub_addr",addresses.at(0));
            saveState.setValue("pub_addr",addresses.at(1));
            saveState.setValue("msg_addr",addresses.at(2));
        }


    saveState.endArray();
    saveState.endGroup();
}

void ArenaUI::on_actionCamera_toggled(bool arg1)
{
    if(arg1){
        //videoStream = new QGstreamerView(sideLayout);
    }
    if(!arg1){
        //videoStream->deleteLater();
    }
}
