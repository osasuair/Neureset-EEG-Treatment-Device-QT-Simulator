#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QListWidgetItem>
#include <ctime>

#include "new_session.h"

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

    void triggerDateChange();

    void shutdown();
    void powerOn();


private:
    Ui::MainWindow *ui;

    bool power = false;
    int stackScreen = 0;

    QTimer *timer;
    QTimer *secondTimer; // Timer
    time_t sys_time; // Current time
    NewSession* newSession;

public slots:
    void disablePlay(bool disable);
    void disablePause(bool disable);
    void disableStop(bool disable);

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
    void on_pushButton_2_clicked();
};
#endif // MAINWINDOW_H
