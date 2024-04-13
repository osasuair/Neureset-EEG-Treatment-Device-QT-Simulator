#ifndef SITEMANAGER_H
#define SITEMANAGER_H

#include <QObject>
#include <QTimer>
#include "qcustomplot.h"
#include <QVector>
#include <QPair>
#include <iostream>
#include <chrono>
#include <QThread>
#include <QMutex>

class SiteManager : public QObject
{
    Q_OBJECT
public:
    explicit SiteManager(QObject *parent = nullptr);

    void setWaveFormGraph(QCustomPlot* waveForm);

    QCustomPlot* waveFormGraph;

    QTimer* sessionTimer;
    int round;
    int site;
    int remainingTime = 60*1000;
    bool inSession = true;
    double baselineBefore;
    double baselineAfter;
    std::vector<QPair<QVector<double>, QVector<double>>> generated_waveforms;
    std::vector<double> dominantFrequencies;

    void reset();
    void createPlot();
    void createWaveforms(std::default_random_engine& re);
    void applyTreatment(int site);

    std::default_random_engine re;

public slots:
    void startNewSessionTimer();
    void onSessionTimeout();
    void startTreatmentPhase();
    void pauseSession();
    void resumeSession();
    void onTreatmentTimerTimeout();
    void onSiteFinished();

signals:
    void sessionOver();
    void completeRound();
    void completeTreatment();
};

#endif // SITEMANAGER_H
