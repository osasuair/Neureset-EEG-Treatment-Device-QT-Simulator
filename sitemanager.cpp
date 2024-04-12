#include "sitemanager.h"
#include "sitemanager.h"


SiteManager::SiteManager(QObject *parent)
    : QObject{parent}
{

    round = 1;
    site = 0;
    baselineBefore = 0;
    baselineAfter = 0;
        \
    for (int i = 0; i < 21; ++i) {
        WorkerThread *thread = new WorkerThread(this);
        connect(thread, &WorkerThread::siteFinished, this, &SiteManager::onSiteFinished);
        workerThreads[i] = thread;
    }

    // Create treatment timer
    sessionTimer = new QTimer(NULL);
    connect(sessionTimer, &QTimer::timeout, this, &SiteManager::onSessionTimeout);

    // Create treatment timer
}

void SiteManager::setWaveFormGraph(QCustomPlot *waveForm)
{
    waveFormGraph = waveForm;
    createWaveforms();
    createPlot();
}

void SiteManager::reset()
{
    round = 1;
    site = 0;
    baselineBefore = 0;
    baselineAfter = 0;
}

void SiteManager::createPlot()
{
    // Retrieve the waveform data
    QVector<double> x = generated_waveforms[0].first;
    std::cout << std::endl;
    QVector<double> y = generated_waveforms[0].second;
    for (int i = 0; i < y.size(); ++i) {
        std::cout << y[i] << " ";
    }

    // Plot the waveform
    waveFormGraph->addGraph();
    waveFormGraph->graph(0)->setData(x, y);
    // give the axes some labels:
    waveFormGraph->xAxis->setLabel("Time");
    waveFormGraph->yAxis->setLabel("Amplitude");
    // set axes ranges, so we see all data:
    waveFormGraph->xAxis->setRange(0, x.last());
    waveFormGraph->yAxis->setRange(-1, 1);
    waveFormGraph->replot();
}

void SiteManager::createWaveforms()
{
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

void SiteManager::applyTreatment(int site)
{
    dominantFrequencies[site] += 5;
}

void SiteManager::startNewSessionTimer()
{
    // Start 21 threads for generating waveforms
    for (int i = 0; i < 21; ++i) {
        workerThreads[i]->start();
    }

    // Start a 60-second timer for the session phase
    sessionTimer->start(60*1000); // 60,000 milliseconds = 60 seconds
    qDebug() << "analyzing waveform";
}

void SiteManager::onSessionTimeout()
{
    qDebug() << "Moving to treatment phase";
    sessionTimer->stop();
    createWaveforms();
    emit completeRound();
}

void SiteManager::startTreatmentPhase()
{
    QTimer::singleShot(1, this, &SiteManager::onTreatmentTimerTimeout);
}

void SiteManager::onTreatmentTimerTimeout()
{
    for(int i =0; i<21; ++i) {
        applyTreatment(i); // Apply treatment at the current site
        createWaveforms();
        qDebug() << "Site processing finished blahh.";
    }

    round++;

    if (round == 6) {
        qDebug() << "all done";
        qDebug() << "baselline b4" <<baselineBefore;
        qDebug() << "baseline after" <<baselineAfter;
    }
    emit completeTreatment();
}

void SiteManager::startNextRound()
{

    // Update UI elements or perform any necessary UI updates for the next round

    if (round < 5) {
        startNewSessionTimer();
        return;
    }else{
//        emit sessionOver();
        qDebug() << "all done";
        qDebug() << "baselline b4" <<baselineBefore;
        qDebug() << "baseline after" <<baselineAfter;
    }

}

void SiteManager::onSiteFinished()
{
    QTimer::singleShot(0, this, [this]()
    {
        createWaveforms();
        qDebug() << "Site processing finished.";
    });
}
