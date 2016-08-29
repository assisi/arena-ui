#include "qdeploy.h"

QDeploy::QDeploy(QWidget *parent) :
    QLabel(parent),
    _simulatorPID(0)
{
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setTextFormat(Qt::PlainText);
    setWordWrap(true);

    _shell = new QProcess;
}

QDeploy::~QDeploy()
{
    _shell->close();
    delete _shell;
}


void QDeploy::setWorkingDirectory(QString dir)
{
    _shell->setWorkingDirectory(dir);
}

void QDeploy::appendText(QString text)
{
    setText(this->text() + text);
}

bool QDeploy::isSimulatorStarted()
{
    QString tempString = settings->value("simulator").toString();
    tempString = tempString.right(tempString.size() - tempString.lastIndexOf("/") - 1);
    tempString = tempString.left(15);

    _shell->start("sh");
    _shell->write("pgrep ");
    _shell->write(tempString.toStdString().c_str());
    _shell->closeWriteChannel();
    _shell->waitForFinished();

    QString out(_shell->readAll());
    if(out.size()) _simulatorPID = out.toLongLong();
    return out.size();
}

void QDeploy::deploy()
{
    if(_shell->state() == QProcess::NotRunning){
        _shell->start("deploy.py", QStringList(assisiFile.name));
        appendText("[arenaUI] Starting 'deploy.py'\n");
    }
    else appendText("[arenaUI] Already running a process\n");
}

void QDeploy::run()
{
    if(_shell->state() == QProcess::NotRunning){
        _shell->start("assisirun.py", QStringList(assisiFile.name));
        appendText("[arenaUI] Starting 'assisirun.py'\n");
    }
    else appendText("[arenaUI] Already running a process\n");
}

void QDeploy::stop()
{
    if(_shell->state() != QProcess::NotRunning) _shell->close();
    else appendText("[arenaUI] Process is not running\n");
}

void QDeploy::collect()
{
    if(_shell->state() == QProcess::NotRunning){
        _shell->start("collect_data.py", QStringList(assisiFile.name));
        appendText("[arenaUI] Starting 'collect_data.py'\n");
    }
    else appendText("[arenaUI] Already running a process\n");
}

void QDeploy::cleanLog()
{

}

void QDeploy::simulatorStart()
{
    if(_shell->state() == QProcess::NotRunning){
        if(isSimulatorStarted()){
            appendText("[Simulator] Simulator is already running (PID: " + QString::number(_simulatorPID) +")\n");
            return;
        }
        _shell->startDetached(settings->value("simulator").toString(), QStringList(), QString(), &_simulatorPID);
        if(!_simulatorPID) appendText("[Simulator] Cannot start: " + settings->value("simulator").toString() + "\n");
        else appendText("[Simulator] New simulator started (PID: " + QString::number(_simulatorPID) +")\n");
    }
    else appendText("[arenaUI] Already running a process\n");

    _shell->start("sim.py", QStringList(assisiFile.arenaFile));
}

void QDeploy::simulatorStop()
{
    if(isSimulatorStarted()){
        _shell->execute(QString("kill ") + QString::number(_simulatorPID));
        appendText("[Simulator] Simulator process is stopped.");
    }
}

void QDeploy::appendOut()
{
    appendText("[SHELL][OUT] ");
    appendText(_shell->readAllStandardOutput());
}

void QDeploy::appendErr()
{
    appendText("[SHELL][ERR] ");
    appendText(_shell->readAllStandardError());
}

void QDeploy::toggleOutput(int state){
    if(state){
        _shell->readAllStandardError();
        _shell->readAllStandardOutput();
        connect(_shell, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
        connect(_shell, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
    }
    else{
        disconnect(_shell, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
        disconnect(_shell, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
    }

}
