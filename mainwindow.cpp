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
    connect(sessionWaitTimer, &QTimer::timeout, [this]() { sessionTimeout(); });

    startDisableTimer();
    startSecondTimer(); // Update every second

    newSession->startSession(sys_time);
    createWaveforms();
    createPlot();


    round = 1;
    site = 0;
    baselineBefore = 0;
    baselineAfter = 0;

    // Create session timer
    sessionTimer = new QTimer(this);
    connect(sessionTimer, &QTimer::timeout, this, &MainWindow::onSessionTimeout);

    // Create treatment timer
    treatmentTimer = new QTimer(this);
    connect(treatmentTimer, &QTimer::timeout, this, &MainWindow::onTreatmentTimerTimeout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSiteFinished() {

    qDebug() << "Site processing finished.";
}




void MainWindow::startNewSessionTimer() {
    // Start 21 threads for generating waveforms
    for (int i = 0; i < 21; ++i) {
        WorkerThread *thread = new WorkerThread(this);
        connect(thread, &WorkerThread::siteFinished, this, &MainWindow::onSiteFinished);
        thread->start();
    }

    // Start a 60-second timer for the session phase
    sessionTimer->start(3000); // 60,000 milliseconds = 60 seconds
    qDebug() << "analyzing waveform";
}


void MainWindow::onSessionTimeout() {
    qDebug() << "Moving to treatment phase";
    sessionTimer->stop();
    createWaveforms();
    startTreatmentPhase(); // Start the treatment phase
}

void MainWindow::startTreatmentPhase(){
    treatmentTimer->start(1000); // 1000 milliseconds = 1 second
}


void MainWindow::onTreatmentTimerTimeout(){
    applyTreatment(site); // Apply treatment at the current site

        // Check if all sites have been treated in this round
        if (site == 0) {
            // Move to the next round
            round++;

            // Reset site counter for the next round
            site = 0;

            // Perform any other actions needed for transitioning to the next round
            startNextRound();
            // Stop the treatment timer until the next round starts
            treatmentTimer->stop();
        } else {
            // Move to the next site for treatment
            site++;
        }
}

void MainWindow::startNextRound() {
    // Reset any variables or perform any necessary cleanup for the current round
    generated_waveforms.clear();

    // Update UI elements or perform any necessary UI updates for the next round

    if (round < 5) {
        startNewSessionTimer();
        return;
    }else{
        qDebug() << "all done";
        qDebug() << baselineBefore;
        qDebug() << baselineAfter;
    }

}


void MainWindow::createWaveforms() {
    const int num_points = 30; // Number of data points to generate
    const double time_step = 0.02; // Time step


    // Seed the random number generator with current time
    unsigned seed = QDateTime::currentDateTime().toTime_t();
    std::default_random_engine re(seed);

    // Storage for randomly selected frequencies and amplitudes for each category
    std::vector<double> frequencies(4);
    std::vector<double> amplitudes(4);

    // Create four different sine waves, one for each category (alpha, beta, theta, delta)
    std::vector<QVector<double>> waveforms;
    waveforms.resize(4);


        // Create the sine waves for each category and store their randomly selected frequencies and amplitudes
        for (int category = 0; category < 4; ++category) {
            double frequency;
            if (category == 0) { // Alpha waves (8-12 Hz)
                std::uniform_real_distribution<double> unif(8.0, 12.0);
                frequency = unif(re);
            } else if (category == 1) { // Beta waves (12-30 Hz)
                std::uniform_real_distribution<double> unif(12.0, 30.0);
                frequency = unif(re);
            } else if (category == 2) { // Theta waves (4-8 Hz)
                std::uniform_real_distribution<double> unif(4.0, 8.0);
                frequency = unif(re);
            } else { // Delta waves (0.5-4 Hz)
                std::uniform_real_distribution<double> unif(0.5, 4.0);
                frequency = unif(re);
            }

            std::uniform_real_distribution<double> unif_amplitude(0.5, 1.0);
            double amplitude = unif_amplitude(re);

            frequencies[category] = frequency;
            amplitudes[category] = amplitude;

            QVector<double> waveform(num_points);
            for (int i = 0; i < num_points; ++i) {
                double t = i * time_step;
                waveform[i] = amplitude * sin(2 * M_PI * frequency * t);
            }
            waveforms[category] = waveform;
        }

    // Print a message indicating the end of waveform generation
    qDebug() << "Waveform generation completed.";

    // Calculate the dominant frequency using the randomly selected frequencies and amplitudes
    double dominant_frequency = (frequencies[0] * std::pow(amplitudes[0], 2) +
                                  frequencies[1] * std::pow(amplitudes[1], 2) +
                                  frequencies[2] * std::pow(amplitudes[2], 2) +
                                  frequencies[3] * std::pow(amplitudes[3], 2)) /
                                 (std::pow(amplitudes[0], 2) +
                                  std::pow(amplitudes[1], 2) +
                                  std::pow(amplitudes[2], 2) +
                                  std::pow(amplitudes[3], 2));

    dominantFrequencies.push_back(dominant_frequency);

    if (round == 1 && site == 0) {
        double sum = 0.0;
        for (double freq : dominantFrequencies) {
            sum += freq;
        }
        baselineBefore = sum / dominantFrequencies.size();
    }
    else if (round == 4 && site == 0){
        double sum = 0.0;
        for (double freq : dominantFrequencies) {
            sum += freq;
        }
        baselineAfter = sum / dominantFrequencies.size();
    }

    // Generate the data points using the dominant frequency and a random amplitude
    QVector<double> x(num_points), y(num_points);
    for (int i = 0; i < num_points; ++i) {
        double t = i * time_step;
        std::uniform_real_distribution<double> unif_amplitude(0.5, 1.0);
        double amplitude = unif_amplitude(re);
        x[i] = t;
        y[i] = amplitude * sin(2 * M_PI * dominant_frequency * t);
    }

    // Store the generated waveform
    generated_waveforms.push_back(QPair<QVector<double>, QVector<double>>(x, y));
}

void MainWindow::applyTreatment(int site){

    dominantFrequencies[site] += 5;
}



void MainWindow::createPlot(){
    // Retrieve the waveform data
    QVector<double> x = generated_waveforms[0].first;
    std::cout << std::endl;
    QVector<double> y = generated_waveforms[0].second;
    for (int i = 0; i < y.size(); ++i) {
        std::cout << y[i] << " ";
    }

    // Plot the waveform
    ui->check123->addGraph();
    ui->check123->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->check123->xAxis->setLabel("Time");
    ui->check123->yAxis->setLabel("Amplitude");
    // set axes ranges, so we see all data:
    ui->check123->xAxis->setRange(0, x.last());
    ui->check123->yAxis->setRange(-1, 1);
    ui->check123->replot();
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
    round = 1;
    site = 0;
    baselineBefore = 0;
    baselineAfter = 0;
    generated_waveforms.clear();
    startNewSessionTimer();
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

