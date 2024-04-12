#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QListWidgetItem>
#include <QStandardItemModel>
#include <QStandardItem>
#include <ctime>
#include <string>
#include <random>
#include <cmath>
#include <QVector>
#include <QPair>
#include <iostream>
#include <chrono>
#include <QThread>
#include <QMutex>

#include "qcustomplot.h"
#include "sessionmanager.h"
#include "log.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum stackScreens {
    OFF,
    MENU,
    NEW_SESSION,
    SESSION_LOG,
    DATE_TIME
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

    void showSessionLog();
    void setupSessionLog();
    void updateLog(int id, time_t time, float before_baseline,float after_baseline);

    void triggerDateChange();

    void shutdown();
    void powerOn();
    void updateBatteryIcon();
    void chargeBattery();

private:
    Ui::MainWindow *ui;

    bool power = false;
    int batteryLevel = 5;
    int stackScreen = 0;
    bool nodesConnected = true;

    QTimer *timer;
    QTimer *secondTimer; // Timer
    time_t sys_time; // Current time
    SessionManager* sessionManager;
    QStandardItemModel *logModel;
    Log log;

public slots:
    void disablePlay(bool disable);
    void disablePause(bool disable);
    void disableStop(bool disable);

    void flashRedLight();
    void flashBlueLight();
    void flashGreenLight();

    bool batteryLowered();

private slots:
    void on_playButton_clicked();
    void on_pauseButton_clicked();
    void on_stopButton_clicked();
    void on_pushButton_clicked();
    void on_menuListWidget_itemClicked(QListWidgetItem *item);
    void on_menuButton_clicked();
    void on_menuUpButton_clicked();
    void on_menuDownButton_clicked();
    void on_sessionButton_clicked();
    void on_pushButton_2_clicked();
    void on_chargeButton_clicked();
    void on_looseConnectionButton_clicked();
    void on_reconnectButton_clicked();
};
#endif // MAINWINDOW_H
