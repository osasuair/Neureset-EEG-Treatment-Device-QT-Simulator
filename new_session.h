#ifndef NEW_SESSION_H
#define NEW_SESSION_H

#include <ctime>
#include <vector>
#include "sitemanager.h"

#include <QProgressBar>
#include <QLCDNumber>
#include "qcustomplot.h"
#include <QTimer>
#include <QString>
#include <QPushButton>
#include <cmath>

struct SessionLog{
    int name;
};

class NewSession
{
public:
    NewSession(QProgressBar *progress, QLCDNumber *lcd, QTimer *timer);

    bool getPlaying() const;
    void setWavePlot(QCustomPlot *wavePlot);

    void clearSession();
    void startSession(time_t start_time);
    void pauseSession();
    void resumeSession();
    void stopSession();
    void timeout();
    SessionLog endSession();

    void updateLCDTime();
    void updateProgressBar();

    bool getComplete() const;

private:
    int progress = 0;
    bool playing = false;
    bool complete = false;
    time_t start_time;
    time_t end_time;

    int secondsRemaining = 5*60;
    int duration = 0; // Track running duration

    QProgressBar *progressBar;
    QLCDNumber *lcdNumber;
    QTimer *waitTimer;
    SiteManager *siteManager;

};

#endif // NEW_SESSION_H
