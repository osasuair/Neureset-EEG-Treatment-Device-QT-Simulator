#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QListWidgetItem>
#include <ctime>
#include <random>
#include <cmath>
#include "new_session.h"
#include "qcustomplot.h"
#include "workerthread.h"
#include <QVector>
#include <QPair>
#include <iostream>
#include <chrono>
#include <QThread>
#include <QMutex>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum stackScreens {
    OFF,
    MENU,
    NEW_SESSION,
    SESSION_LOG,
    TIME
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void startNewSession();
    void startDisableTimer();
    void sessionTimeout();
    void startSecondTimer();
    void endNewSession();

    void shutdown();
    void powerOn();
    void createPlot();
    void createWaveforms();
    void applyTreatment(int site);
    std::vector<QPair<QVector<double>, QVector<double>>> generated_waveforms;
    std::vector<double> dominantFrequencies;
    void generateWaveformsThread();
    QTimer* sessionTimer;


private:
    Ui::MainWindow *ui;

    bool power = false;
    int stackScreen = 0;

    QTimer *timer;
    QTimer *secondTimer; // Timer
    time_t sys_time; // Current time
    NewSession* newSession;
    QTimer* treatmentTimer;
    int round;
    int site;
    double baselineBefore;
    double baselineAfter;
public slots:
    void disablePlay(bool disable);
    void disablePause(bool disable);
    void disableStop(bool disable);
    void onSiteFinished();
signals:
    void setDisabled(bool disable);

private slots:
    void on_playButton_clicked();
    void on_pauseButton_clicked();
    void on_stopButton_clicked();
    void on_pushButton_clicked();
    void on_menuListWidget_itemClicked(QListWidgetItem *item);
    void on_menuButton_clicked();
    void on_menuUpButton_clicked();
    void on_menuDownButton_clicked();
    void startNewSessionTimer();
    void onSessionTimeout();
    void startTreatmentPhase();
    void onTreatmentTimerTimeout();
    void startNextRound();
};
#endif // MAINWINDOW_H
