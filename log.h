#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <vector>
#include <iostream>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QListView>

#include "sessiondata.h"

using namespace std;

class Log : public QObject
{
    Q_OBJECT
public:
    explicit Log(QObject *parent = nullptr);
    ~Log();
    void addSession(int id, time_t now, float before_baseline, float after_baseine);
    void addSession(SessionData * session);

    // only display date and time from sessions
    void printSession();

    // display date, time and baseline frequencies
    void printToPC();


    void setListView(QListView *list);

    void setupHeader();




signals:

private:
    vector<SessionData*> sessionArr;
    QStandardItemModel *logModel;
    QListView *logView;


};



#endif // LOG_H
