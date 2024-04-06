#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sys_time = time(NULL);
    QTimer *sessionWaitTimer = new QTimer(this);
    newSession = new NewSession(ui->sessionProgressBar, ui->sessionClock, sessionWaitTimer);
    connect(sessionWaitTimer, &QTimer::timeout, [this]() { sessionTimeout(); });

    startDisableTimer();
    startSecondTimer(); // Update every second

    newSession->startSession(sys_time);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startDisableTimer()
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]() {
        disablePlay(newSession->getPlaying());
        disablePause(!newSession->getPlaying() || newSession->getComplete());
        disableStop(newSession->getComplete());
    });
    timer->start(500);
}

void MainWindow::sessionTimeout()
{
    newSession->timeout();
    shutdown();
}

void MainWindow::startSecondTimer()
{
    secondTimer = new QTimer(this);
    connect(secondTimer, &QTimer::timeout, [this]() {
        newSession->updateLCDTime(); // Update LCD time in NewSession
        newSession->updateProgressBar();
    });
    secondTimer->start(1000);
}

void MainWindow::shutdown()
{

}

void MainWindow::disablePlay(bool disable){
    emit ui->playButton->setDisabled(disable);
}

void MainWindow::disablePause(bool disable){
    emit ui->pauseButton->setDisabled(disable);
}


void MainWindow::disableStop(bool disable){
    emit ui->stopButton->setDisabled(disable);
}

void MainWindow::on_playButton_clicked()
{
    newSession->resumeSession();
}


void MainWindow::on_pauseButton_clicked()
{
    newSession->pauseSession();
}


void MainWindow::on_stopButton_clicked()
{
    newSession->stopSession();
}

