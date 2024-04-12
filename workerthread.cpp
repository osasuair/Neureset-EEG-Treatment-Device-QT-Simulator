#include "workerthread.h"
#include "mainwindow.h"

WorkerThread::WorkerThread(QObject *parent) : QThread(parent)
{

}

void WorkerThread::run()
{
    if (mainWindow != nullptr) {

        QMutexLocker locker(&mutex);
//        mainWindow->createWaveforms(); // Call createWaveforms() function from MainWindow

    }

    // Emit siteFinished() signal when the task is finished
    emit siteFinished();
}
