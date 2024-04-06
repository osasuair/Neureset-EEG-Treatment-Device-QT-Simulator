#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QTimer>
#include <ctime>

#include "new_session.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void startDisableTimer();
    void sessionTimeout();
    void startSecondTimer();
    void shutdown();

private:
    Ui::MainWindow *ui;

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
};
#endif // MAINWINDOW_H
