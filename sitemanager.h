#ifndef SITEMANAGER_H
#define SITEMANAGER_H

#include <QObject>
#include <QTimer>
#include "qcustomplot.h"
#include "workerthread.h"
#include <QVector>
#include <QPair>
#include <iostream>
#include <chrono>
#include <QThread>
#include <QMutex>

class MainWindow;

class SiteManager : public QObject
{
    Q_OBJECT
public:
    explicit SiteManager(QObject *parent = nullptr);

    void setWaveFormGraph(QCustomPlot* waveForm);

    QCustomPlot* waveFormGraph;

    QTimer* treatmentTimer;
    QTimer* sessionTimer;
    int round;
    int site;
    double baselineBefore;
    double baselineAfter;
    std::vector<QPair<QVector<double>, QVector<double>>> generated_waveforms;
    std::vector<double> dominantFrequencies;
    WorkerThread* workerThreads[21];

    void reset();
    void createPlot();
    void createWaveforms();
    void applyTreatment(int site);

public slots:
    void startNewSessionTimer();
    void onSessionTimeout();
    void startTreatmentPhase();
    void onTreatmentTimerTimeout();
    void startNextRound();
    void onSiteFinished();

signals:
    void sessionOver();
    void completeRound();
    void completeTreatment();

private:
    MainWindow *mainWindow; // Pointer to MainWindow if needed
};

#endif // SITEMANAGER_H
