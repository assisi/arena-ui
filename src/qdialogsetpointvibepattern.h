#ifndef QDIALOGSETPOINTVIBEPATTERN_H
#define QDIALOGSETPOINTVIBEPATTERN_H

#include <QDialog>
#include <QList>
#include <QByteArray>

class QGraphicsItem;
class QAbstractButton;

namespace Ui {
class QDialogSetpointVibePattern;
}

class QDialogSetpointVibePattern : public QDialog
{
    Q_OBJECT

public:
    explicit QDialogSetpointVibePattern(QWidget *parent, const QList<QGraphicsItem *>& group);
    ~QDialogSetpointVibePattern();

public slots:
    // Override accept to implement appropriate behavior
    virtual void accept();

private:
    Ui::QDialogSetpointVibePattern *ui;
    QList<QGraphicsItem *> m_group;
    bool m_done = false;

    const int period_min = 100;
    const int period_max = 60000;
    const int frequency_min = 1;
    const int frequency_max = 1000;
    const int amplitude_min = 0;
    const int amplitude_max = 50;

    void sendSetPoint();

private slots:

    // Connected automagically by Qt
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_radioButton_off_clicked(void);
    void on_radioButton_on_clicked(void);

};

#endif // QDIALOGSETPOINTVIBEPATTERN_H
