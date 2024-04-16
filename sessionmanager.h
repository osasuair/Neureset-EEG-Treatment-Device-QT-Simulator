#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <ctime>
#include <vector>

#include <QObject>
#include <QProgressBar>
#include <QLCDNumber>
#include <QTimer>
#include <QString>
#include <QPushButton>
#include <cmath>

#include "log.h"
#include "sitemanager.h"

class SessionManager: public QObject
{
    Q_OBJECT

public:
    explicit SessionManager(QObject *parent = nullptr);
    SessionManager(QProgressBar *progress, QLCDNumber *lcd, QTimer *timer, Log*);

    bool getPlaying() const;
    void setWavePlot(QCustomPlot *wavePlot);

    void clearSession();
    void startSession(time_t start_time);
    void pauseSession();
    void resumeSession();
    void stopSession();
    void timeout();  // Session Timeout function

    void secondUpdates();
    void updateLCDTime();
    void updateProgressBar();

    bool getComplete() const;

public slots:
    void endSession();
    void roundComplete();
    void treatmentComplete();

signals:
    bool lowerBattery();
    void flashBlueLight();
    void flashRedLight();
    void flashGreenLight();

private:
    static int id;
    int progress = 0;
    bool playing = false;
    bool complete = false;
    time_t sys_time;
    time_t start_time;

    int secondsRemaining = 305;  // 5 minutes and 5 seconds for LCD display

    QProgressBar *progressBar;
    QLCDNumber *lcdNumber;
    QTimer *waitTimer;
    QTimer *flashTimer;
    QDateTime *time;
    Log *log;
    SiteManager *siteManager;
};

#endif // SESSIONMANAGER_H
