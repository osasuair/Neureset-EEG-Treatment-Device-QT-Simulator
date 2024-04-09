#ifndef NEW_SESSION_H
#define NEW_SESSION_H

#include <ctime>
#include <vector>

#include <QProgressBar>
#include <QLCDNumber>
#include <QTimer>
#include <QString>
#include <QPushButton>
#include <cmath>
#include "log.h"

struct SessionLog{
    int name;
    time_t end;
};

class NewSession
{
public:
    NewSession(QProgressBar *progress, QLCDNumber *lcd, QTimer *timer, Log*);

    bool getPlaying() const;

    void clearSession();
    void startSession(time_t start_time);
    void pauseSession();
    void resumeSession();
    void stopSession();
    void timeout();
    void endSession();

    void updateLCDTime();
    void updateProgressBar();

    bool getComplete() const;

private:
    static int id;
    int progress = 0;
    bool playing = false;
    bool complete = false;
    time_t start_time;
    time_t end_time;
    Log *log;

    int secondsRemaining = 5*60;
    int duration = 0; // Track running duration

    QProgressBar *progressBar;
    QLCDNumber *lcdNumber;
    QTimer *waitTimer;
};

#endif // NEW_SESSION_H
