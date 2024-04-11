#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include <QMutex>

class MainWindow;

class WorkerThread : public QThread
{
    Q_OBJECT

public:
    explicit WorkerThread(QObject *parent = nullptr); // Constructor
    mutable QMutex mutex;

signals:
    void siteFinished(); // Signal emitted when the thread finishes its task

protected:
    void run() override; // Override the run() method

private:

    MainWindow *mainWindow; // Pointer to MainWindow if needed
};

#endif // WORKERTHREAD_H
