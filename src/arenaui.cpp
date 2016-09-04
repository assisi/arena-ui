#include <QThread>
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QMessageBox>

#include "arenaui.h"
#include "ui_arenaui.h"

using namespace zmqData;

// -------------------------------------------------------------------------------

ArenaUI::ArenaUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArenaUI)
{
    QLocale::setDefault(QLocale::C); //override for localization to use C locale

    loadConfig();
    ui->setupUi(this);
    ui->actionToggleLog->setChecked(settings->value("log_on").toBool());

    // CASU TREE TAB
    ui->casuTree->addAction(ui->actionPlot_selected_in_same_trend);
    ui->casuTree->addAction(ui->actionPlot_selected_in_different_trends);
    ui->casuTree->header()->setSortIndicator(0,Qt::AscendingOrder);

    ui->groupTree->addAction(ui->actionPlot_selected_in_same_trend);
    ui->groupTree->addAction(ui->actionPlot_selected_in_different_trends);

    auto tempLambda = [&](){
        if(ui->casuTree->selectedItems().size() || ui->groupTree->selectedItems().size()){
            ui->actionPlot_selected_in_same_trend->setEnabled(true);
            ui->actionPlot_selected_in_different_trends->setEnabled(true);
        }
        else{
            ui->actionPlot_selected_in_same_trend->setEnabled(false);
            ui->actionPlot_selected_in_different_trends->setEnabled(false);
        }
    };
    connect(ui->casuTree, &QTreeWidget::itemSelectionChanged, tempLambda);
    connect(ui->groupTree, &QTreeWidget::itemSelectionChanged, tempLambda);


    // TREND TAB SCROLLABLE LAYOUT

    auto tempWidget = new QWidget;
    auto tempScroll = new QScrollArea;

    tempScroll->setWidget(tempWidget);
    tempScroll->setWidgetResizable(true);
    _trendTab = new QVBoxLayout(tempWidget);

    ui->tabTrend->setLayout(new QVBoxLayout);
    ui->tabTrend->layout()->addWidget(tempScroll);

    // DEPLOYMENT TAB
    // - set custom flow layout
    for(auto& widget : ui->deployButtons->children())
        ui->deployButtons->layout()->removeWidget(qobject_cast<QWidget *>(widget));
    auto tempLayout = new FlowLayout;
    delete ui->deployButtons->layout();
    ui->deployButtons->setLayout(tempLayout);
    auto tempList = ui->deployButtons->children();
    tempList.removeLast();
    qSort(tempList.begin(),tempList.end(),[](QObject *obj1, QObject *obj2){
        // objects have names "_#objectName" where # is wanted position
        return obj1->objectName().at(1) < obj2->objectName().at(1);
    });
    for(auto& widget : tempList)
        ui->deployButtons->layout()->addWidget(qobject_cast<QWidget *>(widget));

    // - set deploy widget in scroll area
    ui->scrollArea->setWidget(ui->deployWidget);
    connect(ui->scrollArea->verticalScrollBar(), &QScrollBar::rangeChanged,[&](int min, int max){
        Q_UNUSED(min)
        ui->scrollArea->verticalScrollBar()->setValue(max);
    });

    // GRAPHICS SCENE
    _arenaScene = new QArenaScene(this);
    _arenaScene->setSceneRect(0,0,800,800);
    ui->arenaSpace->setScene(_arenaScene);
    ui->arenaSpace->setDragMode(QGraphicsView::RubberBandDrag);
    new QGraphicsViewZoom(ui->arenaSpace);

    auto click_handler = new MouseClickHandler(_arenaScene, this);
    _arenaScene->installEventFilter(click_handler);

    connect(ui->arenaSpace, &QGraphicsView::customContextMenuRequested, this, &ArenaUI::customContextMenu);

    // - 30fps update
    _sceneUpdate = new QTimer(this);
    _sceneUpdate->start(33);
    // NOTE: QGraphicsScene::update() has default value
    connect(_sceneUpdate, &QTimer::timeout, [&](){ _arenaScene->update(); });
}

ArenaUI::~ArenaUI()
{
    delete ui;
}

void ArenaUI::sortGraphicsScene()
{
    auto tempSelection = _arenaScene->selectedItems(); // save active selection
    QPainterPath pp;
    pp.addRect(_arenaScene->sceneRect()); // select all -- selectedItems() doesnt return group children which is a case with items()
    _arenaScene->setSelectionArea(pp);

    for(int k = 0; k < _arenaScene->selectedItems().size(); k++)
        _arenaScene->selectedItems()[k]->setZValue(k+1);

    for(int k = 0; k+1 < _arenaScene->selectedItems().size(); k++)
        for(int i = k+1; i < _arenaScene->selectedItems().size(); i++){
            auto item1 = sCast(_arenaScene->selectedItems()[k]);
            auto item2 = sCast(_arenaScene->selectedItems()[i]);

            auto path1 = std::move(item1->completeShape());
            auto path2 = std::move(item2->completeShape());

            if(path1.intersects(path2)){
                int z1 = item1->zValue();
                int z2 = item2->zValue();
                if(z1 > z2) std::swap(z1,z2);

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
    for(auto& item : tempSelection) item->setSelected(true); // after saving items and groups, return selection as was before

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

        return QObject::eventFilter(obj, event);;
    }
    else
    if (event->type() == QEvent::GraphicsSceneMouseMove){
        for(auto& item : selectedList) item->setSelected(true);
        drag_true = true;
        scene_->update();

        return QObject::eventFilter(obj, event);;
    }
    else
    if (event->type() == QEvent::GraphicsSceneMouseRelease)
    {
        for(auto& item : selectedList) item->setSelected(true);

        auto mouse_click = static_cast<QGraphicsSceneMouseEvent *>(event);
        auto itemAtMouse= scene_->itemAt(mouse_click->scenePos().x(),mouse_click->scenePos().y(), QTransform());

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

        return QObject::eventFilter(obj, event);;
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
    auto loadFile = QFileDialog::getOpenFileName(this,tr("Open Arena configuration file"), settings->value("arenaFolder").toString(), tr("All(*.arenaUI *assisi);;Project(*.assisi);;Session(*.arenaUI)"));
    if(!loadFile.size()) return;

    while(_trendTab->count()){
        ((QCustomPlot*)_trendTab->itemAt(0)->widget())->close();
        _trendTab->removeWidget(_trendTab->itemAt(0)->widget());
    }

    _arenaScene->clear();
    ui->casuTree->clear();
    ui->groupTree->clear();
    /*Create new selectionTreeItem*/{
        auto tempTreeItem = new QSelectionTreeItem(_arenaScene);
        ui->groupTree->addTopLevelItem(tempTreeItem);
        _arenaScene->setTreeItem(tempTreeItem);
    }

// FIXME: github issue #39
    if(loadFile.endsWith(".assisi")){
        assisiFile.name = loadFile;
        _assisiNode = YAML::LoadFile(assisiFile.name.toStdString());

        ui->arenaLabel->setText(QString::fromStdString(_assisiNode["arena"].as<std::string>()));
        ui->neighLabel->setText(QString::fromStdString(_assisiNode["dep"].as<std::string>()));
        ui->deployLabel->setText(QString::fromStdString(_assisiNode["nbg"].as<std::string>()));

        ui->deployWidget->setWorkingDirectory(assisiFile.name.left(assisiFile.name.lastIndexOf('/')));

        assisiFile.arenaFile = assisiFile.name.left(assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(_assisiNode["arena"].as<std::string>());
        assisiFile.depFile = assisiFile.name.left(assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(_assisiNode["dep"].as<std::string>());
        assisiFile.ngbFile = assisiFile.name.left(assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(_assisiNode["nbg"].as<std::string>());

        auto arenaNode = YAML::LoadFile(assisiFile.arenaFile.toStdString());
        QList<QString> layers;
        for(auto it=arenaNode.begin(); it!=arenaNode.end(); it++) layers.append(QString::fromStdString(it->first.as<std::string>()));

        if(layers.size() > 1) assisiFile.arenaLayer = QInputDialog::getItem(this,tr("Select arena layer"),"",QStringList(layers));
        else assisiFile.arenaLayer = layers[0];

        progress.setMaximum(arenaNode[assisiFile.arenaLayer.toStdString()].size());
        progress.show();
        progress.move(ui->arenaSpace->mapToGlobal(QPoint(400-progress.width()/2,400-progress.height()/2)));

        //for(auto it=arenaNode[assisiFile.arenaLayer.toStdString()].begin(); it!=arenaNode[assisiFile.arenaLayer.toStdString()].end(); it++){
        for(auto casu : arenaNode[assisiFile.arenaLayer.toStdString()]){
            auto name = QString::fromStdString(casu.first.as<std::string>());
            QPointF coordinates;
            coordinates.setX(400 + 10 * arenaNode[assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["x"].as<double>());
            coordinates.setY(400 - 10 * arenaNode[assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["y"].as<double>());
            double yaw = arenaNode[assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["yaw"].as<double>();

            auto tempZMQ = new QCasuZMQ(this, name);
            auto tempTreeItem = new QCasuTreeItem(tempZMQ);
            auto tempSceneItem = new QCasuSceneItem(coordinates, yaw, tempZMQ);

            connect(tempZMQ, &QCasuZMQ::connectMsg, ui->deployWidget, &QDeploy::append);

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

        ui->arenaLabel->setText(QString::fromStdString(_assisiNode["arena"].as<std::string>()));
        ui->neighLabel->setText(QString::fromStdString(_assisiNode["dep"].as<std::string>()));
        ui->deployLabel->setText(QString::fromStdString(_assisiNode["nbg"].as<std::string>()));

        ui->deployWidget->setWorkingDirectory(assisiFile.name.left(assisiFile.name.lastIndexOf('/')));

        assisiFile.arenaFile = assisiFile.name.left(assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(_assisiNode["arena"].as<std::string>());
        assisiFile.depFile = assisiFile.name.left(assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(_assisiNode["dep"].as<std::string>());
        assisiFile.ngbFile = assisiFile.name.left(assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(_assisiNode["nbg"].as<std::string>());

        auto arenaNode = YAML::LoadFile(assisiFile.arenaFile.toStdString());
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
                auto casuName = loadSession.value("casuName").toString();
                auto key = dCast(loadSession.value("key").toInt());
                toAdd.append(linker[casuName]->getBuffer(key));
            }
            if(toAdd.size()){
                auto tempWidget = new QTrendPlot(ui->casuTree, ui->groupTree);
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

    auto itemList= _arenaScene->selectedItems();
    if(itemList.size()<2) return;
    _arenaScene->clearSelection();

    auto tempTreeGroup = new QCasuTreeGroup();
    auto tempSceneGroup = new QCasuSceneGroup();

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
    auto itemList= _arenaScene->selectedItems();
    for(auto& item : itemList)
        if(sCast(item)->isGroup()){
            sgCast(item)->removeFromGroup(item->childItems());
            sCast(item)->deleteTreeItem();
            _arenaScene->destroyItemGroup(sCast(item));
        }
    this->sortGraphicsScene();
}

void ArenaUI::on_actionConnect_triggered()
{
    bool error = false;
    if (_arenaScene->selectedItems().size() != 1) error = true; //Check if excactly one object is selected
    if(sCast(_arenaScene->selectedItems().first())->isGroup()) error = true; // Check if object is single casu (no children)
    if(error){
        QMessageBox msgBox;
        msgBox.setWindowTitle("ERROR");
        msgBox.setText("Please select one CASU.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    auto item = siCast(_arenaScene->selectedItems().first());
    auto addrDiag = new QDialogConnect(this, item->getZmqObject()->getAddresses(), item->getZmqObject()->getName());
    if(addrDiag->exec()){
        item->getZmqObject()->setAddresses(addrDiag->getAddresses());
    }
}

void ArenaUI::on_actionToggleLog_triggered()
{
    ui->actionToggleLog->setChecked(settings->value("log_on").toBool());
    auto question = QString("Are you sure you want to turn ") + (settings->value("log_on").toBool() ? QString("OFF") : QString("ON")) + QString(" logging?");
    auto reply = QMessageBox::question(this, "Toggle Log", question , QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes)settings->setValue("log_on", !settings->value("log_on").toBool());
    ui->actionToggleLog->setChecked(settings->value("log_on").toBool());
}

void ArenaUI::on_actionPlot_selected_in_same_trend_triggered()
{
    auto tempWidget = new QTrendPlot(ui->casuTree, ui->groupTree);
    _trendTab->addWidget(tempWidget);

    tempWidget->addSelectedGraphs();
    tempWidget->setWindowTitle(assisiFile.arenaLayer);
}

void ArenaUI::on_actionPlot_selected_in_different_trends_triggered()
{
    QList<zmqBuffer *> bufferList;

    for(int k=0; k < ui->casuTree->topLevelItemCount(); k++)
        bufferList.append(tCast(ui->casuTree->topLevelItem(k))->getBuffers());
    for(int k=0; k < ui->groupTree->topLevelItemCount(); k++)
        bufferList.append(tCast(ui->groupTree->topLevelItem(k))->getBuffers());

    for(auto& buffer : bufferList){
        QList<zmqBuffer*> tempList;
        tempList.append(buffer);

        auto tempWidget = new QTrendPlot(ui->casuTree, ui->groupTree);
        _trendTab->addWidget(tempWidget);

        tempWidget->addGraphList(tempList);
        tempWidget->setWindowTitle(assisiFile.arenaLayer);
    }
}

void ArenaUI::on_actionSettings_triggered()
{
    auto settingsDiag = new QDialogSettings(this);
    settingsDiag->exec();
}

void ArenaUI::customContextMenu(QPoint pos)
{
    auto menu = new QMenu(); //no parent because it inherits background image from QGraphicsView
    QAction* tempAction;

    menu->setAttribute(Qt::WA_DeleteOnClose);

    bool error_single = false;
    if (_arenaScene->selectedItems().size() != 1) error_single = true; //Check if excactly one object is selected
    if (_arenaScene->selectedItems().size() == 1)
        if(_arenaScene->selectedItems()[0]->childItems().size()) error_single = true; // Check if object is single casu (no children)

    bool error_selected = !_arenaScene->selectedItems().size();

    // FIXME: Qt 5.6 QMenu::addAction accepts Qt5 style connect (possible lambda expressions)
    tempAction = menu->addAction(settings->value("IR_on").toBool() ? "Hide proximity sensors" : "Show proximity sensors");
    connect(tempAction, &QAction::triggered, [&](){
        settings->setValue("IR_on",!settings->value("IR_on").toBool());
    });
    tempAction = menu->addAction(settings->value("temp_on").toBool() ? "Hide temperature sensors" : "Show temperature sensors");
    connect(tempAction, &QAction::triggered, [&](){
        settings->setValue("temp_on",!settings->value("temp_on").toBool());
    });
    tempAction = menu->addAction(settings->value("vibr_on").toBool() ? "Hide vibration marker" : "Show vibration marker");
    connect(tempAction, &QAction::triggered, [&](){
        settings->setValue("vibr_on",!settings->value("vibr_on").toBool());
    });
    tempAction = menu->addAction(settings->value("air_on").toBool() ? "Hide airflow marker" : "Show airflow marker");
    connect(tempAction, &QAction::triggered, [&](){
        settings->setValue("air_on",!settings->value("air_on").toBool());
    });
    tempAction = menu->addAction(settings->value("avgTime_on").toBool() ? "Hide avg. sample time" : "Show avg. sample time");
    connect(tempAction, &QAction::triggered, [&](){
        settings->setValue("avgTime_on",!settings->value("avgTime_on").toBool());
    });

    menu->addSeparator();

    tempAction = menu->addAction("Group selected", this,SLOT(on_actionGroup_triggered()));
    if(error_selected) tempAction->setEnabled(false);
    tempAction = menu->addAction("Ungroup selected", this,SLOT(on_actionUngroup_triggered()));
    if(error_selected) tempAction->setEnabled(false);
    menu->addSeparator();
    tempAction = menu->addAction("Set connection", this,SLOT(on_actionConnect_triggered()));
    if(error_single) tempAction->setEnabled(false);

    auto sendMenu = new QMenu("Setpoint");
    if(error_selected) sendMenu->setEnabled(false);
    menu->addMenu(sendMenu);

    auto signalMapper = new QSignalMapper(menu);

    tempAction = sendMenu->addAction("Temperature",signalMapper,SLOT(map()));
    signalMapper->setMapping(tempAction,"Temperature");
    tempAction = sendMenu->addAction("Vibration",signalMapper,SLOT(map()));
    signalMapper->setMapping(tempAction,"Vibration");
    tempAction = sendMenu->addAction("Airflow",signalMapper,SLOT(map()));
    signalMapper->setMapping(tempAction,"Airflow");
    tempAction = sendMenu->addAction("LED",signalMapper,SLOT(map()));
    signalMapper->setMapping(tempAction,"LED");
    tempAction = sendMenu->addAction("IR Proximity",signalMapper,SLOT(map()));
    signalMapper->setMapping(tempAction,"IR Proximity");

    connect(signalMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped), this,[&](QString actuator){
        auto dialog = new QDialogSetpoint(this, actuator,_arenaScene->selectedItems());
        if(dialog->exec())
            for(auto& item : _arenaScene->selectedItems())
                sCast(item)->sendSetpoint(dialog->getMessage());
    });

    menu->popup(ui->arenaSpace->mapToGlobal(pos));
}

void ArenaUI::groupSave(QSettings *saveState, const QList<QGraphicsItem *> &group, const QString &groupName)
{
    saveState->beginWriteArray(groupName);
    int k = 0;
    for(auto& item : group){
        saveState->setArrayIndex(k++);
        if(!sCast(item)->isGroup()) saveState->setValue("casuName",siCast(item)->getZmqObject()->getName());
        else groupSave(saveState, item->childItems(), "group");
    }
    saveState->endArray();
}

QList<QGraphicsItem *> ArenaUI::groupLoad(YAML::Node *arenaNode, QSettings *loadState, const int &groupSize, QMap<QString, QCasuZMQ *> *linkMap, QProgressBar *progress)
{
    QList<QGraphicsItem *> returnGroup;
    for(int k=0; k < groupSize; k++){
        loadState->setArrayIndex(k);
        progress->setValue(progress->value()+1);

        if(loadState->childKeys().size()){
            auto name = loadState->value("casuName").toString();
            QPointF coordinates;
            coordinates.setX(400 + 10 * (*arenaNode)[assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["x"].as<double>());
            coordinates.setY(400 - 10 * (*arenaNode)[assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["y"].as<double>());
            double yaw = (*arenaNode)[assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["yaw"].as<double>();

            auto tempZMQ = new QCasuZMQ(this, name);
            auto tempTreeItem = new QCasuTreeItem(tempZMQ);
            auto tempSceneItem = new QCasuSceneItem(coordinates, yaw, tempZMQ);

            connect(tempZMQ, &QCasuZMQ::connectMsg, ui->deployWidget, &QDeploy::append);

            tempTreeItem->setSceneItem(tempSceneItem);
            tempSceneItem->setTreeItem(tempTreeItem);

            ui->casuTree->addTopLevelItem(tempTreeItem);
            _arenaScene->addItem(tempSceneItem);
            tempTreeItem->setHidden(true);

            linkMap->insert(name, tempZMQ);

            progress->setValue(progress->value()+1);
            returnGroup.append(tempSceneItem);
        }
        else{
            int tempSize = loadState->beginReadArray("group");
            progress->setMaximum(progress->maximum() + tempSize);

            auto tempTreeGroup = new QCasuTreeGroup();
            auto tempSceneGroup = new QCasuSceneGroup();
            tempTreeGroup->setSceneItem(tempSceneGroup);
            tempSceneGroup->setTreeItem(tempTreeGroup);

            auto itemList= std::move(groupLoad(arenaNode, loadState, tempSize, linkMap, progress));
            tempSceneGroup->addToGroup(itemList);

            ui->groupTree->addTopLevelItem(tempTreeGroup);
            _arenaScene->addItem(tempSceneGroup);

            tempTreeGroup->setSelected(0);
            loadState->endArray();
            returnGroup.append(tempSceneGroup);
        }

        QApplication::processEvents();
    }
   return returnGroup;
}

void ArenaUI::on_actionSave_triggered()
{
    auto saveFile = QFileDialog::getSaveFileName(this,tr("Save As"),settings->value("arenaFolder").toString(),tr("*.arenaUI"));

    if(!saveFile.size())return;
    if(!saveFile.endsWith(".arenaUI")) saveFile += ".arenaUI";
    if(QFile(saveFile).exists())QFile(saveFile).remove();

    QSettings saveState(saveFile,QSettings::IniFormat);

    saveState.setValue("assisiFile",assisiFile.name);
    saveState.setValue("arenaLayer",assisiFile.arenaLayer);

    //save CASU graphics scene

    auto tempSelection = _arenaScene->selectedItems(); // save active selection
    QPainterPath pp;
    pp.addRect(_arenaScene->sceneRect()); // select all -- selectedItems() doesnt return group children which is a case with items()
    _arenaScene->setSelectionArea(pp);

    saveState.beginGroup("sceneHierarchy");
    groupSave(&saveState, _arenaScene->selectedItems(),"main");
    saveState.endGroup();

    pp = pp.subtracted(pp);
    pp.addRect(0,0,0,0);
    _arenaScene->setSelectionArea(pp);
    for(auto& item : tempSelection) item->setSelected(true); // after saving items and groups, return selection as was before

    //save trend position and graphs

    saveState.beginGroup("trendGraphs");
    saveState.beginWriteArray("plot");
    for(int k=0; k < _trendTab->count() ; k++){
        auto tempPlot = (QTrendPlot*) _trendTab->itemAt(k)->widget();
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
    for(auto& item : _arenaScene->items())
        if(!sCast(item)->isGroup()){
            saveState.setArrayIndex(index++);
            saveState.setValue("casuName",siCast(item)->getZmqObject()->getName());
            auto addresses = std::move(siCast(item)->getZmqObject()->getAddresses());
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
