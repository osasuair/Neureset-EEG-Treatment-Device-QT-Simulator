#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,log()
{
    ui->setupUi(this);

    logModel = new QStandardItemModel(this);
    sys_time = time(NULL);
    QTimer *sessionWaitTimer = new QTimer(this);
    newSession = new NewSession(ui->sessionProgressBar, ui->sessionClock, sessionWaitTimer);
    connect(sessionWaitTimer, &QTimer::timeout, [this]() { sessionTimeout(); });

    setupSessionLog();
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

void MainWindow::updateLog(int id, time_t time, float before_baseline,float after_baseline){
    SessionData *session = new SessionData (id, time, before_baseline, after_baseline);
    log.addSession(session);

    // Get the data from the SessionData object
    string ID_Str = to_string(session->getID());
    string dateStr = session->getDateStr();

    // Convert the data strings to QStrings
    QString qID_Str = QString::fromStdString(ID_Str);
    QString qDateStr = QString::fromStdString(dateStr);

    // Add a row to the model
    QList<QStandardItem*> rowItems;
    rowItems.append(new QStandardItem(qID_Str)); // Add the date string as the first column
    rowItems.append(new QStandardItem(qDateStr)); // Add the data string as the second column

    logModel->appendRow(rowItems);

}

void MainWindow::setupSessionLog(){

    // Header Values to be displayed in SEssion LOg
    QString dateHeader = QString::fromStdString("DATE");
    QString IDstr = QString::fromStdString("ID");

    // Add a row to the model
    QList<QStandardItem*> rowItems;
    rowItems.append(new QStandardItem(dateHeader)); // Add the date string as the first column
    rowItems.append(new QStandardItem(IDstr)); // Add the data string as the second column
    logModel->appendRow(rowItems);
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

