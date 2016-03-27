#ifndef QDEPLOY_H
#define QDEPLOY_H

#include <QLabel>
#include <QProcess>
#include <QWindow>

#include <QThread>

#include "globalHeader.h"

class QDeploy : public QLabel
{
    Q_OBJECT
public:
    QDeploy(QWidget *parent = 0);

    void setWorkingDirectory(QString dir);

private:
    QProcess* shell;
    QProcess* simulatorProcess;
    QProcess* spawner;

    void appendText(QString text);

signals:

public slots:
    void deploy();
    void run();
    void stop();
    void collect();
    void cleanLog();

    void simulatorStart();
    void simulatorStop();

private slots:
    void appendOut();
    void appendErr();
};

#endif // QDEPLOY_H
