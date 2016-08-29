#include "qdeploy.h"

QDeploy::QDeploy(QWidget *parent) :
    QLabel(parent),
    _simulatorPID(0)
{
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->setTextFormat(Qt::PlainText);
    this->setWordWrap(true);

    _shell = new QProcess;
    _simulatorProcess = new QProcess;
    _spawner = new QProcess;

}

QDeploy::~QDeploy()
{
    _shell->close();
    _spawner->close();
    _simulatorProcess->close();
    delete _shell;
    delete _spawner;
    delete _simulatorProcess;
}


void QDeploy::setWorkingDirectory(QString dir)
{
    _shell->setWorkingDirectory(dir);
}

void QDeploy::appendText(QString text)
{
    this->setText(this->text() + text);
}

void QDeploy::deploy()
{
    if(_shell->state() == QProcess::NotRunning){
        _shell->start("deploy.py", QStringList(assisiFile.name));
        this->appendText("[arenaUI] Starting 'deploy.py'\n");
    }
    else this->appendText("[arenaUI] Already running a program\n");
}

void QDeploy::run()
{
    if(_shell->state() == QProcess::NotRunning){
        _shell->start("assisirun.py", QStringList(assisiFile.name));
        this->appendText("[arenaUI] Starting 'assisirun.py'\n");
    }
    else this->appendText("[arenaUI] Already running a program\n");
}

void QDeploy::stop()
{
    if(_shell->state() != QProcess::NotRunning) _shell->close();
    else this->appendText("[arenaUI] Program is not running\n");
}

void QDeploy::collect()
{
    if(_shell->state() == QProcess::NotRunning){
        _shell->start("collect_data.py", QStringList(assisiFile.name));
        this->appendText("[arenaUI] Starting 'collect_data.py'\n");
    }
    else this->appendText("[arenaUI] Already running a program\n");
}

void QDeploy::cleanLog()
{

}

void QDeploy::simulatorStart()
{
    if(_simulatorPID != 0){
        this->appendText("[arenaUI] Simulator PID exist: " + QString::number(_simulatorPID) +"\n");
        this->appendText("[arenaUI] Stop simulator to reset PID!");
        return;
    }
    if(_simulatorProcess->state() == 0)_simulatorProcess->startDetached(settings->value("simulator").toString(), QStringList(), QString(), &_simulatorPID);
    if(_simulatorPID == 0){
        this->appendText("[arenaUI] Cannot start: " + settings->value("simulator").toString() + "\n");
    }
    _spawner->start("sim.py", QStringList(assisiFile.arenaFile));
}

void QDeploy::simulatorStop()
{
    if(_simulatorPID != 0) _simulatorProcess->execute(QString("kill ") + QString::number(_simulatorPID));
    _simulatorPID = 0;
}

void QDeploy::appendOut()
{
    QProcess* proc = (QProcess*) this->sender();
    if(proc == _shell) this->appendText("[SHELL][OUT] ");
    else this->appendText("[SIMULATOR][OUT] ");
    this->appendText(proc->readAllStandardOutput());
}

void QDeploy::appendErr()
{
    QProcess* proc = (QProcess*) this->sender();
    if(proc == _shell) this->appendText("[SHELL][ERR] ");
    else this->appendText("[SIMULATOR][ERR] ");
    this->appendText(proc->readAllStandardError());
}

void QDeploy::toggleOutput(int state){
    if(state){
        _shell->readAllStandardError();
        _shell->readAllStandardOutput();
        _spawner->readAllStandardError();
        _spawner->readAllStandardOutput();
        connect(_shell, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
        connect(_shell, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
        connect(_spawner, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
        connect(_spawner, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
    }
    else{
        disconnect(_shell, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
        disconnect(_shell, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
        disconnect(_spawner, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
        disconnect(_spawner, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
    }

}
