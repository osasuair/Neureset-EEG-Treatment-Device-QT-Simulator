#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,log()
{
    // ui setup
    ui->setupUi(this);
    sys_time = time(NULL);
    ui->dateTimeEdit->setDateTime(QDateTime::fromSecsSinceEpoch(sys_time));
    ui->stackedWidget->setCurrentIndex(OFF);

    // Log setup
    logModel = new QStandardItemModel(this);
    ui->sessionLogListView->setModel(logModel);
    log.setListView(ui->sessionLogListView);

    // Session setup
    QTimer *sessionWaitTimer = new QTimer(this);
    sessionManager = new SessionManager(ui->sessionProgressBar, ui->sessionClock, sessionWaitTimer, &log);
    sessionManager->setWavePlot(ui->waveFormGraph);
    connect(sessionWaitTimer, &QTimer::timeout, [this]() { sessionTimeout(); });

    connect(sessionManager, &SessionManager::flashBlueLight, this, &MainWindow::flashBlueLight);
    connect(sessionManager, &SessionManager::flashRedLight, this, &MainWindow::flashRedLight);
    connect(sessionManager, &SessionManager::flashGreenLight, this, &MainWindow::flashGreenLight);
    connect(sessionManager, &SessionManager::lowerBattery, this, &MainWindow::batteryLowered);

    // Start timers
    startDisableTimer();
    startSecondTimer(); // Update every second
}

MainWindow::~MainWindow()
{
    delete timer;
    delete secondTimer;
    delete sessionManager;
    delete logModel;
    delete ui;
}

/**
 * @brief MainWindow::startDisableTimer
 * Start the timer to disable buttons
*/
void MainWindow::startDisableTimer()
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]() {
        disablePlay(sessionManager->getPlaying());
        disablePause(!sessionManager->getPlaying() || sessionManager->getComplete());
        disableStop(sessionManager->getComplete());
    });
}

/**
 * @brief MainWindow::sessionTimeout
 * Session timeout function when the session timeout timer runs out
*/
void MainWindow::sessionTimeout()
{
    sessionManager->timeout();
    shutdown();
}

/**
 * @brief MainWindow::startSecondTimer
 * Start the second timer
*/
void MainWindow::startSecondTimer()
{
    secondTimer = new QTimer(this);
    connect(secondTimer, &QTimer::timeout, [this]() {
        sessionManager->secondUpdates();
    });
}

/**
 * @brief MainWindow::startNewSession
 * Start a new session
*/
void MainWindow::startNewSession()
{
    stackScreen = NEW_SESSION;
    ui->stackedWidget->setCurrentIndex(NEW_SESSION);
    timer->start(500);
    secondTimer->start(1000);
    sessionManager->startSession(sys_time);

}

/**
 * @brief MainWindow::endNewSession
 * End the current session
*/
void MainWindow::endNewSession()
{
    sessionManager->stopSession();
    timer->stop();
    secondTimer->stop();

    disablePlay(true);
    disableStop(true);
    disablePause(true);
}

/**
 * @brief MainWindow::shutdown
 * Shutdown the device
*/
void MainWindow::shutdown()
{
    if(stackScreen == NEW_SESSION){
       endNewSession();
    }
    power = false;
    stackScreen = OFF;
    ui->stackedWidget->setCurrentIndex(OFF);
}

/**
 * @brief MainWindow::powerOn
 * Power on the device
*/
void MainWindow::powerOn()
{
    if (batteryLevel == 0)
        return;
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

void MainWindow::flashRedLight()
{
    ui->redLight->setStyleSheet("background-color: #FFA07A; border: 1px solid black"); // Lighter red color
    QTimer::singleShot(150, this, [=](){ ui->redLight->setStyleSheet("background-color: rgb(200, 0, 0); border: 1px solid black"); });
}

void MainWindow::flashBlueLight()
{
    ui->blueLight->setStyleSheet("background-color: #87CEEB; border: 1px solid black"); // Lighter blue color
    QTimer::singleShot(300, this, [=](){ ui->blueLight->setStyleSheet("background-color: rgb(0, 0, 200); border: 1px solid black"); });
}

void MainWindow::flashGreenLight()
{
    ui->greenLight->setStyleSheet("background-color: #90EE90; border: 1px solid black"); // Lighter green color
    QTimer::singleShot(1000, this, [=](){ ui->greenLight->setStyleSheet("background-color: rgb(0, 180, 0); border: 1px solid black"); });
}

/**
 * @brief MainWindow::batteryLowered
 * @return bool returns true if battery is dead
 * Lower the battery level
*/
bool MainWindow::batteryLowered()
{
    --batteryLevel;

    updateBatteryIcon();
    return batteryLevel == 0;
}

/**
 * @brief MainWindow::chargeBattery
 * Charge the battery to full
*/
void MainWindow::chargeBattery()
{
    batteryLevel = 5;
    updateBatteryIcon();
}

/**
 * @brief MainWindow::updateBatteryIcon
 * Update the battery icon based on the battery level
*/
void MainWindow::updateBatteryIcon() {
    QPixmap mypix;
    if(batteryLevel <= 0) {
        batteryLevel = 0;
        if (power) {
            qDebug() << "Shutting down! Charge Battery";
            shutdown();
        }
        mypix = QPixmap(":/images/noBattery.png");
    } else if (batteryLevel <= 2) {
        mypix = QPixmap(":/images/lowBattery.png");
    } else if (batteryLevel <= 4) {
        mypix = QPixmap(":/images/mediumBattery.png");
    } else {
        mypix = QPixmap(":/images/fullBattery.png");
    }
    ui->battery->setPixmap(mypix);
}

void MainWindow::on_playButton_clicked()
{
    sessionManager->resumeSession();
}

void MainWindow::on_pauseButton_clicked()
{
    sessionManager->pauseSession();
}

void MainWindow::on_stopButton_clicked()
{
    sessionManager->stopSession();
}

// Power Button
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


void MainWindow::on_chargeButton_clicked()
{
    chargeBattery();
}

/**
 * @brief MainWindow::on_looseConnectionButton_clicked
 * Loose connection button clicked
*/
void MainWindow::on_looseConnectionButton_clicked()
{
    if(stackScreen == NEW_SESSION && sessionManager->getPlaying()){
       qDebug("Site Disconnected!");
       sessionManager->pauseSession();
    }
    else qDebug() <<  "Session not running";
}

/**
 * @brief MainWindow::on_reconnectButton_clicked
 * Reconnect button clicked
*/
void MainWindow::on_reconnectButton_clicked()
{
    if (stackScreen == NEW_SESSION && sessionManager->getPlaying()){
        qDebug("Site Disconnected!");
        sessionManager->resumeSession();
     }
    else qDebug() <<  "Session not running";
}

