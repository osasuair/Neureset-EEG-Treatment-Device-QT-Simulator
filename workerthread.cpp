#include "workerthread.h"
#include "mainwindow.h"

WorkerThread::WorkerThread(QObject *parent) : QThread(parent)
{

}

void WorkerThread::run()
{
    if (mainWindow != nullptr) {
        qDebug() << "before.";
        QMutexLocker locker(&mutex);
//        mainWindow->createWaveforms(); // Call createWaveforms() function from MainWindow
        qDebug() << ".";
    }

    // Emit siteFinished() signal when the task is finished
    emit siteFinished();
}
