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

    connect(flashTimer, &QTimer::timeout, [this]() {emit flashRedLight(); qDebug("beep"); });
    connect(siteManager, &SiteManager::sessionOver, this, &SessionManager::endSession);
    connect(siteManager, &SiteManager::completeRound, this, &SessionManager::roundComplete);
    connect(siteManager, &SiteManager::completeTreatment, this, &SessionManager::treatmentComplete);
}


bool SessionManager::getPlaying() const
{
    return playing;
}

void SessionManager::setWavePlot(QCustomPlot *wavePlot)
{
    siteManager->setWaveFormGraph(wavePlot);
}

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

void SessionManager::pauseSession()
{
    playing = false;
    flashTimer->start(1000);;
    waitTimer->start(1000*30);
    siteManager->pauseSession();
}

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

void SessionManager::timeout()
{
    playing = false;
    complete = false;
    qInfo("time OUTT!!!");
}

void SessionManager::endSession()
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

void SessionManager::roundComplete()
{
    siteManager->startTreatmentPhase();
    emit flashGreenLight();
}

void SessionManager::treatmentComplete()
{
    emit flashGreenLight();
    siteManager->createPlot();
    //siteManager->generated_waveforms.clear();
    if(siteManager->round <6){
        siteManager->startNewSessionTimer();
        return;
    }
    endSession();


}

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

void SessionManager::updateLCDTime()
{
    int minutes = secondsRemaining / 60;
    int seconds = secondsRemaining % 60;

    // Format minutes and seconds into "MM:SS" format
    QString displayText = QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));

    // Update lcdNumber
    lcdNumber->display(displayText);
}

void SessionManager::updateProgressBar()
{
    if (progressBar->value() != progress)
    {
        emit progressBar->setValue(progress);
    }
}

bool SessionManager::getComplete() const
{
    return complete;
}



