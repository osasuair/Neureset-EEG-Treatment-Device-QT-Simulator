#include "new_session.h"

NewSession::NewSession(QProgressBar *progress, QLCDNumber *lcd, QTimer *timer)
{
    progressBar = progress;
    lcdNumber = lcd;
    waitTimer = timer;
    siteManager = new SiteManager();

    updateLCDTime();
}


bool NewSession::getPlaying() const
{
    return playing;
}

void NewSession::setWavePlot(QCustomPlot *wavePlot)
{
    siteManager->setWaveFormGraph(wavePlot);
}

void NewSession::startSession(time_t start_time)
{
    siteManager->reset();

    complete = false;
    playing = true;

    // Update start_time
    siteManager->startNewSessionTimer();

    this->start_time = start_time;
    // Reset duration
    duration = 0;
    // Set secondsRemaining to 5min
    secondsRemaining = 322;
}

void NewSession::pauseSession()
{
    playing = false;
    waitTimer->start(1000*10);
}

void NewSession::resumeSession()
{
    if (complete){
        startSession(start_time);
    }
    waitTimer->stop();
    playing = true;
}

void NewSession::stopSession()
{
    if(waitTimer->isActive()) {
        waitTimer->stop();
    }
    playing = false;
    complete = true;
}

void NewSession::timeout()
{
    playing = false;
    complete = false;
    qInfo("time OUTT!!!");
}

SessionLog NewSession::endSession()
{
    // End the session and return session log
    complete = true;

    SessionLog s;
    return s;
}

void NewSession::updateLCDTime()
{
    // Update values
    duration++;
    if (playing) {
        secondsRemaining--;

        progress = 100-std::round(float(secondsRemaining)/321*100);

    }

    int minutes = secondsRemaining / 60;
    int seconds = secondsRemaining % 60;

    // Format minutes and seconds into "MM:SS" format
    QString displayText = QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));

    // Update lcdNumber
    lcdNumber->display(displayText);
}

void NewSession::updateProgressBar()
{
    if (progressBar->value() != progress)
    {
        emit progressBar->setValue(progress);
    }
}

bool NewSession::getComplete() const
{
    return complete;
}


