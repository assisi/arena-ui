#include "qdeploy.h"

QDeploy::QDeploy(QWidget *parent) :
    QLabel(parent),
    simulatorPID(0)
{
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->setTextFormat(Qt::PlainText);
    this->setWordWrap(true);

    shell = new QProcess;
    simulatorProcess = new QProcess;
    spawner = new QProcess;

}

QDeploy::~QDeploy()
{
    shell->close();
    spawner->close();
    simulatorProcess->close();
    delete shell;
    delete spawner;
    delete simulatorProcess;
}


void QDeploy::setWorkingDirectory(QString dir)
{
    shell->setWorkingDirectory(dir);
}

void QDeploy::appendText(QString text)
{
    this->setText(this->text() + text);
}

void QDeploy::deploy()
{
    if(shell->state() == QProcess::NotRunning){
        shell->start("deploy.py", QStringList(assisiFile.name));
        this->appendText("[arenaUI] Starting 'deploy.py'\n");
    }
    else this->appendText("[arenaUI] Already running a program\n");
}

void QDeploy::run()
{
    if(shell->state() == QProcess::NotRunning){
        shell->start("assisirun.py", QStringList(assisiFile.name));
        this->appendText("[arenaUI] Starting 'assisirun.py'\n");
    }
    else this->appendText("[arenaUI] Already running a program\n");
}

void QDeploy::stop()
{
    if(shell->state() != QProcess::NotRunning) shell->close();
    else this->appendText("[arenaUI] Program is not running\n");
}

void QDeploy::collect()
{
    if(shell->state() == QProcess::NotRunning){
        shell->start("collect_data.py", QStringList(assisiFile.name));
        this->appendText("[arenaUI] Starting 'collect_data.py'\n");
    }
    else this->appendText("[arenaUI] Already running a program\n");
}

void QDeploy::cleanLog()
{

}

void QDeploy::simulatorStart()
{
    if(simulatorPID != 0){
        this->appendText("[arenaUI] Simulator already running (" + QString::number(simulatorPID) +")\n");
        return;
    }
    if(simulatorProcess->state() == 0)simulatorProcess->startDetached(settings->value("simulator").toString(), QStringList(), QString(), &simulatorPID);
    if(!simulatorProcess->waitForStarted(1000)){
        simulatorProcess->terminate();
        this->appendText("[arenaUI] Cannot start: " + settings->value("simulator").toString() + "\n");
    }
    spawner->start("sim.py", QStringList(assisiFile.arenaFile));
}

void QDeploy::simulatorStop()
{
    if(simulatorPID != 0) simulatorProcess->execute(QString("kill -9 ") + QString::number(simulatorPID));
    simulatorPID = 0;
}

void QDeploy::appendOut()
{
    QProcess* proc = (QProcess*) this->sender();
    if(proc == shell) this->appendText("[SHELL][OUT] ");
    else this->appendText("[SIMULATOR][OUT] ");
    this->appendText(proc->readAllStandardOutput());
}

void QDeploy::appendErr()
{
    QProcess* proc = (QProcess*) this->sender();
    if(proc == shell) this->appendText("[SHELL][ERR] ");
    else this->appendText("[SIMULATOR][ERR] ");
    this->appendText(proc->readAllStandardError());
}

void QDeploy::toggleOutput(int state){
    if(state){
        shell->readAllStandardError();
        shell->readAllStandardOutput();
        spawner->readAllStandardError();
        spawner->readAllStandardOutput();
        connect(shell, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
        connect(shell, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
        connect(spawner, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
        connect(spawner, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
    }
    else{
        disconnect(shell, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
        disconnect(shell, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
        disconnect(spawner, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
        disconnect(spawner, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
    }

}
