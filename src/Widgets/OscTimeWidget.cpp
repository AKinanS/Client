#include "OscTimeWidget.h"

#include "Global.h"

#include "DatabaseManager.h"

#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtCore/QDebug>

OscTimeWidget::OscTimeWidget(QWidget* parent)
    : QWidget(parent),
      fps(0), paused(false), timestamp(0)
{
    setupUi(this);

    this->setVisible(false);
    this->labelLoop->setVisible(false);
    this->labelPlay->setVisible(false);
    this->labelPause->setVisible(false);

    if (DatabaseManager::getInstance().getConfigurationByName("DisableInAndOutPoints").getValue() == "true")
    {
        this->labelOscInTime->setVisible(false);
        this->labelOscOutTime->setVisible(false);
        this->labelOscTime->setStyleSheet("font-size: 16px;");
        this->labelOscTime->setFixedWidth(this->labelOscTime->width() + 20);
        this->labelOscTime->setGeometry(this->labelOscTime->geometry().x() - 10, this->labelOscTime->geometry().y(),
                                        this->labelOscTime->geometry().width(), this->labelOscTime->geometry().height());
    }
}

void OscTimeWidget::reset()
{
    this->timestamp = 0;
    this->progressBarOscTime->reset();
    this->labelOscTime->setText(Osc::DEFAULT_TIME);
    this->labelOscInTime->setText(Osc::DEFAULT_TIME);
    this->labelOscOutTime->setText(Osc::DEFAULT_TIME);

    this->setVisible(false);
}

void OscTimeWidget::setTime(int currentFrame)
{
    if (this->fps == 0)
        return;

    this->setVisible(true);

    double currentTime = currentFrame * (1.0 / this->fps);

    this->labelOscTime->setText(convertToTimecode(currentTime));

    if (this->timestamp == 0) // First time.
        QTimer::singleShot(200, this, SLOT(checkState()));

    this->timestamp = QDateTime::currentMSecsSinceEpoch();
}

void OscTimeWidget::setInOutTime(int seek, int length)
{
    if (this->fps == 0)
        return;

    this->setVisible(true);

    double inTime = seek * (1.0 / this->fps);
    double outTime = (seek + length) * (1.0 / this->fps);

    this->labelOscInTime->setText(convertToTimecode(inTime));
    this->labelOscOutTime->setText(convertToTimecode(outTime));

    this->progressBarOscTime->setRange(seek, seek + length);
}

void OscTimeWidget::setProgress(int currentFrame)
{
    if (this->fps == 0)
        return;

    this->setVisible(true);

    this->progressBarOscTime->setValue(currentFrame);
}

void OscTimeWidget::setFramesPerSecond(int fps)
{
    this->fps = fps;
}

void OscTimeWidget::setPaused(bool paused)
{
    this->paused = paused;

    if (this->paused)
    {
        this->labelPlay->setVisible(false);
        this->labelPause->setVisible(true);
    }
    else
    {
        this->labelPlay->setVisible(true);
        this->labelPause->setVisible(false);
    }
}

void OscTimeWidget::setLoop(bool loop)
{
    this->labelLoop->setVisible(loop);
}

QString OscTimeWidget::convertToTimecode(double time)
{
    int hour;
    int minutes;
    int seconds;
    int frames;

    QString smpteFormat;

    hour = time / 3600;
    minutes = (time - hour * 3600) / 60;
    seconds = time - hour * 3600 - minutes * 60;
    frames = (time - hour * 3600 - minutes * 60 - seconds) * this->fps;

    return smpteFormat.sprintf("%02d:%02d:%02d:%02d", hour, minutes, seconds, frames);
}

void OscTimeWidget::checkState()
{
    qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();

    if (!this->paused && (currentTimestamp - this->timestamp) >= 200)
    {
        reset();

        return;
    }

    QTimer::singleShot(200, this, SLOT(checkState()));
}

void OscTimeWidget::setCompactView(bool compactView)
{
    if (compactView)
    {
        this->labelOscTime->setVisible(!compactView);
        if (DatabaseManager::getInstance().getConfigurationByName("DisableInAndOutPoints").getValue() == "false")
        {
            this->labelOscInTime->setVisible(!compactView);
            this->labelOscOutTime->setVisible(!compactView);
        }

        this->labelLoop->move(Osc::COMPACT_LOOP_X, Osc::COMPACT_LOOP_Y);
        this->labelPlay->move(Osc::COMPACT_PLAY_X, Osc::COMPACT_PLAY_Y);
        this->labelPause->move(Osc::COMPACT_PAUSE_X, Osc::COMPACT_PAUSE_Y);
        this->progressBarOscTime->move(Osc::COMPACT_PROGRESSBAR_X, Osc::COMPACT_PROGRESSBAR_Y);
    }
    else
    {
        this->labelOscTime->setVisible(!compactView);
        if (DatabaseManager::getInstance().getConfigurationByName("DisableInAndOutPoints").getValue() == "false")
        {
            this->labelOscInTime->setVisible(!compactView);
            this->labelOscOutTime->setVisible(!compactView);
        }

        this->labelLoop->move(Osc::DEFAULT_LOOP_X, Osc::DEFAULT_LOOP_Y);
        this->labelPlay->move(Osc::DEFAULT_PLAY_X, Osc::DEFAULT_PLAY_Y);
        this->labelPause->move(Osc::DEFAULT_PAUSE_X, Osc::DEFAULT_PAUSE_Y);
        this->progressBarOscTime->move(Osc::DEFAULT_PROGRESSBAR_X, Osc::DEFAULT_PROGRESSBAR_Y);
    }
}
