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

    // TODO: Set input limit info programatically

    // TODO: Initialize lineEdit box values with current setpoints
    // (requires the resolution of Issue #96)
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
    
    if (ui->radioButton_on->isChecked())
    {
        // Turning on!
        
        // Fill message header
        message.push_back(QString("VibrationPattern").toLocal8Bit());
        message.push_back(QString("On").toLocal8Bit());
        
        // Fill message data
        AssisiMsg::VibrationPattern vp;
        try
        {
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
    else
    {
        // Turn actuator off
        message.push_back(QString("Speaker").toLocal8Bit());
        message.push_back(QString("Off").toLocal8Bit());
        AssisiMsg::VibrationSetpoint v;
        v.set_amplitude(0);
        v.set_freq(1);

        string msg_str = v.SerializeAsString();
        message.push_back(QByteArray(msg_str.c_str(),msg_str.length()));

        for (auto& item : m_group)
        {
            sCast(item)->sendSetpoint(message);
        }
        m_done = true;
    }


}

void QDialogSetpointVibePattern::on_buttonBox_clicked(QAbstractButton *button)
{

    if (button->text().compare(ui->buttonBox->button(QDialogButtonBox::Cancel)->text()) == 0)
    {
        m_done = true;
    }
    else if (button->text().compare(ui->buttonBox->button(QDialogButtonBox::Apply)->text()) == 0)
    {
        this->sendSetPoint();
        // The user hit Apply, so we're not done
        m_done = false;
    }
    else if (button->text().compare(ui->buttonBox->button(QDialogButtonBox::Ok)->text()) == 0)
    {
        this->sendSetPoint();
    }
}

void QDialogSetpointVibePattern::on_radioButton_off_clicked(void)
{
    this->ui->lineEdit_periods->setDisabled(true);
    this->ui->lineEdit_frequencies->setDisabled(true);
    this->ui->lineEdit_amplitudes->setDisabled(true);
}

void QDialogSetpointVibePattern::on_radioButton_on_clicked(void)
{
    this->ui->lineEdit_periods->setDisabled(false);
    this->ui->lineEdit_frequencies->setDisabled(false);
    this->ui->lineEdit_amplitudes->setDisabled(false);
}
