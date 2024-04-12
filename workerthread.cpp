#include "workerthread.h"
#include "mainwindow.h"

WorkerThread::WorkerThread(QObject *parent) : QThread(parent)
{

}

void WorkerThread::run()
{
    emit siteFinished();
}
