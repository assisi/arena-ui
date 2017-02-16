#include <string>

#include <boost/tokenizer.hpp>

#include <QGraphicsItem>

#include "qdialogsetpointvibepattern.h"
#include "ui_qdialogsetpointvibepattern.h"

#include "utils.h"
#include "qcasusceneitem.h"
#include "qcasutreeitem.h"
#include "dev_msgs.pb.h"


QDialogSetpointVibePattern::QDialogSetpointVibePattern(QWidget *parent, const QList<QGraphicsItem *>& group) :
    QDialog(parent),
    ui(new Ui::QDialogSetpointVibePattern),
    m_group(group)
{
    ui->setupUi(this);

    QCasuSceneItem *tempItem = NULL;

    if (!isGroup(group))
    {
        tempItem = siCast(group.first());
    }

    // TODO: Initialize dialog with current setpoints if only one CASU is selected
    // TODO: Initialize dialog with defaults if group is selected

}

QDialogSetpointVibePattern::~QDialogSetpointVibePattern()
{
    delete ui;
}

void QDialogSetpointVibePattern::accept()
{
    if (m_done)
    {
        QDialog::accept();
    }
}

void QDialogSetpointVibePattern::sendSetPoint()
{
    using std::string;

    m_done = false;
    QList<QByteArray> message;

    // Fill message header
    message.push_back(QString("VibrationPattern").toLocal8Bit());
    message.push_back(QString(ui->radioButton_on->isChecked() ? "On" : "Off").toLocal8Bit());

    // Fill message data
    try
    {
        AssisiMsg::VibrationPattern vp;

        // Periods
        string tmp_str = ui->lineEdit_periods->text().toStdString();
        boost::tokenizer<> tok(tmp_str);
        for (auto token: tok)
        {
            int p = std::stoi(token);
            if (isInBounds(p, period_min, period_max))
            {
                vp.add_vibe_periods(p);
            }
            else
            {
                throw std::invalid_argument("Period out of bounds.");
            }
        }

        // Frequencies
        int num_frequencies = 0;
        tmp_str = ui->lineEdit_frequencies->text().toStdString();
        tok.assign(tmp_str);
        for (auto token: tok)
        {
            int f = std::stoi(token);
            if (isInBounds(f, frequency_min, frequency_max))
            {
                vp.add_vibe_freqs(f);
            }
            else
            {
                throw std::invalid_argument("Frequency out of bounds");
            }
        }

        // Amplitudes
        tmp_str = ui->lineEdit_amplitudes->text().toStdString();
        tok.assign(tmp_str);
        for (auto token: tok)
        {
            int a = std::stoi(token);
            if (isInBounds(a, amplitude_min, amplitude_max))
            {
                vp.add_vibe_amps(a);
            }
            else
            {
                throw std::invalid_argument("Amplitude out of bounds");
            }
        }

        // Check input sizes
        if (vp.vibe_periods_size() != vp.vibe_freqs_size() ||
            vp.vibe_freqs_size() != vp.vibe_amps_size())
        {
            throw std::invalid_argument("Input size mismatch");
        }

        // Send the message
        string msg_str = vp.SerializeAsString();
        message.push_back(QByteArray(msg_str.c_str(),msg_str.length()));

        for (auto& item : m_group)
        {
            sCast(item)->sendSetpoint(message);
        }
        m_done = true;
    }
    catch (std::invalid_argument& e)
    {
        QMessageBox err(QMessageBox::Critical, "Input Error",
                        (string("Please check your inputs:\n")
                        + e.what()).c_str());
        err.exec();
    }


}

void QDialogSetpointVibePattern::on_buttonBox_clicked(QAbstractButton *button)
{
    if (button->text() == QString("&Cancel"))
    {
        m_done = true;
    }
    else if (button->text() == QString("Apply"))
    {
        this->sendSetPoint();
        // The user hit Apply, so we're not done
        m_done = false;
    }
    else if (button->text() == QString("&OK"))
    {
        this->sendSetPoint();
    }
}

