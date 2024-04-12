#ifndef NEW_SESSION_H
#define NEW_SESSION_H

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

class NewSession: public QObject
{
    Q_OBJECT

public:
    explicit NewSession(QObject *parent = nullptr);
    NewSession(QProgressBar *progress, QLCDNumber *lcd, QTimer *timer, Log*);

    bool getPlaying() const;
    void setWavePlot(QCustomPlot *wavePlot);

    void clearSession();
    void startSession(time_t start_time);
    void pauseSession();
    void resumeSession();
    void stopSession();
    void timeout();

    void secondUpdates();
    void updateLCDTime();
    void updateProgressBar();

    bool getComplete() const;

public slots:
    void endSession();
    void roundComplete();
    void treatmentComplete();

signals:
    void lowerBattery();
    void flashBlueLight();
    void flashRedLight();
    void flashGreenLight();

private:
    static int id;
    int progress = 0;
    bool playing = false;
    bool complete = false;
    time_t start_time;
    time_t end_time;

    int secondsRemaining = 5*60;

    QProgressBar *progressBar;
    QLCDNumber *lcdNumber;
    QTimer *waitTimer;
    QDateTime *time;
    Log *log;
    SiteManager *siteManager;
};

#endif // NEW_SESSION_H
