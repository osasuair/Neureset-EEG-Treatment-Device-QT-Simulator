#include "new_session.h"

int NewSession::id = 0;

NewSession::NewSession(QObject *parent):
QObject{parent}
{
}

NewSession::NewSession(QProgressBar *progress, QLCDNumber *lcd, QTimer *timer, Log* collection):
    progressBar(progress),
    lcdNumber(lcd),
    waitTimer(timer),
    log(collection),
    siteManager(new SiteManager())
{
    updateLCDTime();

    flashTimer = new QTimer(this);

    connect(flashTimer, &QTimer::timeout, [this]() {emit flashRedLight(); qDebug("beep"); });
    connect(siteManager, &SiteManager::sessionOver, this, &NewSession::endSession);
    connect(siteManager, &SiteManager::completeRound, this, &NewSession::roundComplete);
    connect(siteManager, &SiteManager::completeTreatment, this, &NewSession::treatmentComplete);
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

void NewSession::pauseSession()
{
    playing = false;
    flashTimer->start(1000);;
    waitTimer->start(1000*30);
    siteManager->pauseSession();
}

void NewSession::resumeSession()
{
    waitTimer->stop();
    flashTimer->stop();

    if (complete){
        startSession(start_time);
    }
    siteManager->resumeSession();
    playing = true;
}

void NewSession::stopSession()
{
    if(waitTimer->isActive()) {
        waitTimer->stop();
        flashTimer->stop();
    }
    siteManager->reset();
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

void NewSession::roundComplete()
{
    siteManager->startTreatmentPhase();
    emit flashGreenLight();
}

void NewSession::treatmentComplete()
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



