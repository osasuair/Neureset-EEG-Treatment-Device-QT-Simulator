#ifndef LOG_H
#define LOG_H

#include <QObject>
#include "sessiondata.h"
#include <vector>
#include <iostream>

using namespace std;

class Log : public QObject
{
    Q_OBJECT
public:
    explicit Log(QObject *parent = nullptr);
    ~Log();
    void addSession(int id, tm* now, float before_baseline, float after_baseine);

    // only display date and time from sessions
    void printSession();

    // display date, time and baseline frequencies
    void printToPC();

signals:

private:
    vector<SessionData*> sessionArr;


};



#endif // LOG_H
