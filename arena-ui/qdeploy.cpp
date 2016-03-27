#include "qdeploy.h"

QDeploy::QDeploy(QWidget *parent) :
    QLabel(parent)
{
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->setTextFormat(Qt::PlainText);
    this->setWordWrap(true);

    shell = new QProcess;
    simulatorProcess = new QProcess;
    spawner = new QProcess;

    connect(shell, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
    connect(shell, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
    connect(spawner, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
    connect(spawner, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
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
    this->appendText(assisiFile.name +"\n");
    if(shell->state() == QProcess::NotRunning) shell->start("deploy.py", QStringList(assisiFile.name));
    else this->appendText("[arenaUI] Already running a program\n");
}

void QDeploy::run()
{
    if(shell->state() == QProcess::NotRunning) shell->start("assisirun.py", QStringList(assisiFile.name));
    else this->appendText("[arenaUI] Already running a program\n");
}

void QDeploy::stop()
{
    if(shell->state() != QProcess::NotRunning) shell->terminate();
    else this->appendText("[arenaUI] Program is not running\n");
}

void QDeploy::collect()
{
    shell->start("collect_data.py", QStringList(assisiFile.name));
}

void QDeploy::cleanLog()
{

}

void QDeploy::simulatorStart()
{
    if(simulatorProcess->state() == 0)simulatorProcess->start(settings->value("simulator").toString());
    if(!simulatorProcess->waitForStarted(1000)){
        simulatorProcess->terminate();
        this->appendText("[arenaUI] Cannot start: " + settings->value("simulator").toString() + "\n");
    }
    spawner->start("sim.py", QStringList(assisiFile.arenaFile));
}

void QDeploy::simulatorStop()
{
    simulatorProcess->terminate();
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

