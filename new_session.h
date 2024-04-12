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

class NewSession: public QObject
{
    Q_OBJECT

public:
    explicit NewSession(QObject *parent = nullptr);
    NewSession(QProgressBar *progress, QLCDNumber *lcd, QTimer *timer, Log*);

    bool getPlaying() const;

    void clearSession();
    void startSession(time_t start_time);
    void pauseSession();
    void resumeSession();
    void stopSession();
    void timeout();
    void endSession();

    void secondUpdates();
    void updateLCDTime();
    void updateProgressBar();

    bool getComplete() const;

signals:
    void lowerBattery();

private:
    static int id;
    int progress = 0;
    bool playing = false;
    bool complete = false;
    time_t start_time;
    time_t end_time;
    Log *log;

    int secondsRemaining = 5*60;

    QProgressBar *progressBar;
    QLCDNumber *lcdNumber;
    QTimer *waitTimer;
    QDateTime *time;
};

#endif // NEW_SESSION_H
