#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,log()
{
    ui->setupUi(this);
    sys_time = time(NULL);
    ui->dateTimeEdit->setDateTime(QDateTime::fromSecsSinceEpoch(sys_time));
    ui->stackedWidget->setCurrentIndex(OFF);

    logModel = new QStandardItemModel(this);
    ui->sessionLogListView->setModel(logModel);
    log.setListView(ui->sessionLogListView);

    QTimer *sessionWaitTimer = new QTimer(this);
    newSession = new NewSession(ui->sessionProgressBar, ui->sessionClock, sessionWaitTimer, &log);
    connect(sessionWaitTimer, &QTimer::timeout, [this]() { sessionTimeout(); });


    startDisableTimer();
    startSecondTimer(); // Update every second
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
        newSession->secondUpdates();
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
    power = false;
    stackScreen = OFF;
    ui->stackedWidget->setCurrentIndex(OFF);
}

void MainWindow::powerOn()
{
    power=true;
    stackScreen = MENU;
    ui->stackedWidget->setCurrentIndex(stackScreen);
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
    cout<<index<<endl;
    if (index == NEW_SESSION-2) {
        startNewSession();
    } else if (index == 2) {
        triggerDateChange();
    }
    else if(index == NEW_SESSION-1){
        cout<<"Session log"<<endl;
        showSessionLog();
    }

}


void MainWindow::showSessionLog(){
    stackScreen = SESSION_LOG;
    ui->stackedWidget->setCurrentIndex(SESSION_LOG);

}


void MainWindow::triggerDateChange() {
    stackScreen = DATE_TIME;
    ui->stackedWidget->setCurrentIndex(DATE_TIME);
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
    log.addSession(1, sys_time, 0, 10);
}


void MainWindow::on_menuDownButton_clicked()
{
    int curr = ui->menuListWidget->currentRow();
    int max = ui->menuListWidget->count();

    if(curr < max-1){
        ui->menuListWidget->setCurrentRow(curr+1);
    }
}


void MainWindow::on_sessionButton_clicked()
{
   log.printToPC();
}

void MainWindow::on_pushButton_2_clicked()
{
    QDateTime datetime = ui->dateTimeEdit->dateTime();
    sys_time = datetime.toTime_t();
}

