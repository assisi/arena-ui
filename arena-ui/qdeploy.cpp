#include "qdeploy.h"

QDeploy::QDeploy(QWidget *parent) :
    QLabel(parent)
{
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->setTextFormat(Qt::PlainText);
    this->setWordWrap(true);

    connect(&shell, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
    connect(&shell, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
}

void QDeploy::setWorkingDirectory(QString dir)
{
    shell.setWorkingDirectory(dir);
}

void QDeploy::appendText(QString text)
{
    this->setText(this->text() + text);
}

void QDeploy::deploy()
{
    if(shell.state() == QProcess::NotRunning){
        shell.start("deploy.py", QStringList(assisiFile));
        if(!shell.waitForStarted(5000))this->appendText("[arenaUI] Failed to run deploy.py\n");
    }
    else this->appendText("[arenaUI] Already running a program\n");
}

void QDeploy::run()
{
    if(shell.state() == QProcess::NotRunning) shell.start("deploy.py", QStringList(assisiFile));
    else this->appendText("[arenaUI] Already running a program\n");
}

void QDeploy::stop()
{
    if(shell.state() != QProcess::NotRunning) shell.terminate();
    else this->appendText("[arenaUI] Program is not running\n");
}

void QDeploy::collect()
{
    shell.start("collect_data.py", QStringList(assisiFile));
}

void QDeploy::cleanLog()
{

}

void QDeploy::appendOut()
{
    this->appendText(shell.readAllStandardOutput());
}

void QDeploy::appendErr()
{
    this->appendText(shell.readAllStandardError());
}

