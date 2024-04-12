#include "sessionmanager.h"

int SessionManager::id = 0;

SessionManager::SessionManager(QObject *parent):
QObject{parent}
{
}

SessionManager::SessionManager(QProgressBar *progress, QLCDNumber *lcd, QTimer *timer, Log* collection):
    progressBar(progress),
    lcdNumber(lcd),
    waitTimer(timer),
    log(collection),
    siteManager(new SiteManager())
{
    updateLCDTime();

    flashTimer = new QTimer(this);

    connect(flashTimer, &QTimer::timeout, [this]() {emit flashRedLight(); qDebug("beep"); });  // Flash red light
    connect(siteManager, &SiteManager::sessionOver, this, &SessionManager::endSession);  // End session
    connect(siteManager, &SiteManager::completeRound, this, &SessionManager::roundComplete);
    connect(siteManager, &SiteManager::completeTreatment, this, &SessionManager::treatmentComplete);
}


bool SessionManager::getPlaying() const
{
    return playing;
}

/**
 * @brief SessionManager::setWavePlot
 * @param wavePlot QCustomPlot* wavePlot
 * Set the wavePlot to the SiteManager
*/
void SessionManager::setWavePlot(QCustomPlot *wavePlot)
{
    siteManager->setWaveFormGraph(wavePlot);
}

/**
 * @brief SessionManager::clearSession
 * @param start_time time_t start time of the session
 * Clear the session and reset the SiteManager
*/
void SessionManager::startSession(time_t start_time)
{
    bool batteryDead = emit lowerBattery();
    if (batteryDead) return;

    complete = false;
    playing = true;
    // Set secondsRemaining to 5min and 5 seconds
    secondsRemaining = 305;
    // Update start_time
    this->start_time = start_time;

    // Update start_time
    siteManager->startNewSessionTimer();
    emit flashBlueLight();
}

/**
 * @brief SessionManager::pauseSession
 * Pause the session and start the waitTimer
*/
void SessionManager::pauseSession()
{
    playing = false;
    flashTimer->start(1000);;
    waitTimer->start(1000*30);
    siteManager->pauseSession();
}

/**
 * @brief SessionManager::resumeSession
 * Resume the session and stop the waitTimer
*/
void SessionManager::resumeSession()
{
    waitTimer->stop();
    flashTimer->stop();

    if (complete){
        startSession(start_time);
    }
    siteManager->resumeSession();
    playing = true;
}

/**
 * @brief SessionManager::stopSession
 * Stop the session and reset the SiteManager
*/
void SessionManager::stopSession()
{
    if(waitTimer->isActive()) {
        waitTimer->stop();
        flashTimer->stop();
    }
    siteManager->reset();
    playing = false;
    complete = true;
}

/**
 * @brief SessionManager::timeout
 * Session has been paused for too long -> timeout the session
*/
void SessionManager::timeout()
{
    playing = false;
    complete = false;
    qInfo("time OUTT!!!");
}

/**
 * @brief SessionManager::endSession
 * End the session and log the session
*/
void SessionManager::endSession()
{
    // End the session and return session log
    log->addSession(id, end_time, siteManager->baselineBefore, siteManager->baselineAfter);
    stopSession();
    secondsRemaining =0;
    progress =100;
    secondUpdates();

    qDebug() << "Session Complete";
    emit flashGreenLight();
    id++;
}

/**
 * @brief SessionManager::roundComplete
 * Round is complete -> start treatment phase
*/
void SessionManager::roundComplete()
{
    siteManager->startTreatmentPhase();
    emit flashGreenLight();
}

/**
 * @brief SessionManager::treatmentComplete
 * Treatment phase is complete -> start new session
*/
void SessionManager::treatmentComplete()
{
    emit flashGreenLight();
    siteManager->createPlot();
    if(siteManager->round <6){
        siteManager->startNewSessionTimer();
        return;
    }
    endSession();
}

/**
 * @brief SessionManager::secondUpdates
 * Update the session every second
*/
void SessionManager::secondUpdates()
{
    // Update values
    if (playing) {
        secondsRemaining--;
        progress = 100-std::round((float(secondsRemaining)/306)*100);
    }
    updateLCDTime();
    updateProgressBar();;
}

/**
 * @brief SessionManager::updateLCDTime
 * Update the LCD time
*/
void SessionManager::updateLCDTime()
{
    int minutes = secondsRemaining / 60;
    int seconds = secondsRemaining % 60;

    // Format minutes and seconds into "MM:SS" format
    QString displayText = QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));

    // Update lcdNumber
    lcdNumber->display(displayText);
}

/**
 * @brief SessionManager::updateProgressBar
 * Update the progress bar
*/
void SessionManager::updateProgressBar()
{
    if (progressBar->value() != progress)
    {
        emit progressBar->setValue(progress);
    }
}

/**
 * @brief SessionManager::getComplete
 * @return bool complete
 * Return if the session is complete
*/
bool SessionManager::getComplete() const
{
    return complete;
}



