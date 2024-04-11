#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sys_time = time(NULL);
    //createPlot();
    QTimer *sessionWaitTimer = new QTimer(this);
    newSession = new NewSession(ui->sessionProgressBar, ui->sessionClock, sessionWaitTimer);
    newSession->setWavePlot(ui->waveFormGraph);
    connect(sessionWaitTimer, &QTimer::timeout, [this]() { sessionTimeout(); });

    startDisableTimer();
    startSecondTimer(); // Update every second

    startNewSession();
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
}

void MainWindow::startNewSession()
{
    stackScreen = NEW_SESSION;
    ui->stackedWidget->setCurrentIndex(NEW_SESSION);
    timer->start(500);
    secondTimer->start(1000);
    newSession->startSession(sys_time);
}

void MainWindow::endNewSession()
{
    newSession->stopSession();
    timer->stop();
    secondTimer->stop();

    disablePlay(true);
    disableStop(true);
    disablePause(true);
}

void MainWindow::shutdown()
{
    if(stackScreen == NEW_SESSION){
       endNewSession();
    }
    stackScreen = OFF;
    ui->stackedWidget->setCurrentIndex(OFF);
}

void MainWindow::powerOn()
{
    power=true;
    stackScreen = MENU;
    ui->stackedWidget->setCurrentIndex(stackScreen);
}

void MainWindow::flashRedLight()
{
    ui->redLight->setStyleSheet("background-color: #FFA07A; border: 1px solid black"); // Lighter red color
    QTimer::singleShot(300, this, [=](){ ui->redLight->setStyleSheet("background-color: rgb(200, 0, 0); border: 1px solid black"); });
}

void MainWindow::flashBlueLight()
{
    ui->blueLight->setStyleSheet("background-color: #87CEEB; border: 1px solid black"); // Lighter blue color
    QTimer::singleShot(300, this, [=](){ ui->blueLight->setStyleSheet("background-color: rgb(0, 0, 200); border: 1px solid black"); });
}

void MainWindow::flashGreenLight()
{
    ui->greenLight->setStyleSheet("background-color: #90EE90; border: 1px solid black"); // Lighter green color
    QTimer::singleShot(300, this, [=](){ ui->greenLight->setStyleSheet("background-color: rgb(0, 180, 0); border: 1px solid black"); });
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


void MainWindow::on_pushButton_clicked()
{
    if(power)
        shutdown();
    else
        powerOn();
}


void MainWindow::on_menuListWidget_itemClicked(QListWidgetItem *item)
{
    int index = ui->menuListWidget->row(item);

    if (index == NEW_SESSION-2) {
        startNewSession();
    }
}


void MainWindow::on_menuButton_clicked()
{
    if(!power)
        return;

    if(stackScreen == MENU) {
        QListWidgetItem *item = ui->menuListWidget->selectedItems().at(0);
        on_menuListWidget_itemClicked(item);
        return;
    }

    if(stackScreen == NEW_SESSION){
       endNewSession();
    }

    ui->stackedWidget->setCurrentIndex((stackScreen = MENU));
}


void MainWindow::on_menuUpButton_clicked()
{
   // Check Bounds
    int curr = ui->menuListWidget->currentRow();

    if(curr > 0){
        ui->menuListWidget->setCurrentRow(curr-1);
    }
}


void MainWindow::on_menuDownButton_clicked()
{
    int curr = ui->menuListWidget->currentRow();
    int max = ui->menuListWidget->count();

    if(curr < max-1){
        ui->menuListWidget->setCurrentRow(curr+1);
    }
}

