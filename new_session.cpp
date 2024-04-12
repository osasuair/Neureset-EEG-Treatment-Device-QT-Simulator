#include "new_session.h"

int NewSession::id = 0;

NewSession::NewSession(QObject *parent):
QObject{parent}
{
    connect(siteManager, &SiteManager::sessionOver, this, &NewSession::endSession);
}

NewSession::NewSession(QProgressBar *progress, QLCDNumber *lcd, QTimer *timer, Log* collection):
    progressBar(progress),
    lcdNumber(lcd),
    waitTimer(timer),
    log(collection),
    siteManager(new SiteManager())
{
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
    emit lowerBattery();
    complete = false;
    playing = true;
    // Set secondsRemaining to 5min and 5 seconds
    secondsRemaining = 306;
    // Update start_time
    this->start_time = start_time;

    // Update start_time
    siteManager->startNewSessionTimer();
    emit flashBlueLight();
}

void NewSession::pauseSession()
{
    playing = false;
    emit flashRedLight();
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

void NewSession::endSession()
{
    // End the session and return session log
    stopSession();
    secondsRemaining =0;
    progress =100;
    secondUpdates();

    qDebug() << "Session Complete";
    emit flashGreenLight();
    log->addSession(id, end_time, siteManager->baselineBefore, siteManager->baselineAfter);
    id++;
}

void NewSession::secondUpdates()
{
    // Update values
    if (playing) {
        secondsRemaining--;
        progress = 100-std::round((float(secondsRemaining)/306)*100);
    }
    updateLCDTime();
    updateProgressBar();;
}

void NewSession::updateLCDTime()
{
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



