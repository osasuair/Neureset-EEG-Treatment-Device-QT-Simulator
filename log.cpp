#include "log.h"

using namespace std;

Log::Log(QObject *parent)
    : QObject{parent},
    sessionArr()
{
     logModel = new QStandardItemModel(this);
     setupHeader();
    
}


Log::~Log(){

    int len = sessionArr.size();
    for (int i   = 0; i < len ;  i++ ){
        delete sessionArr[i];
    }
}

// buch of sessiondata pointers

void Log::setupHeader(){

    // Header Values to be displayed in SEssion LOg
    string header = "ID     DATE";
    QString dateHeader = QString::fromStdString(header);


    // Add a row to the model
    QList<QStandardItem*> rowItems;
    rowItems.append(new QStandardItem(dateHeader)); // Add the date string as the first column
    logModel->appendRow(rowItems);

}

void Log::addSession (int id, time_t now, float before_baseline, float after_baseline){
    SessionData *session = new SessionData(id, now, before_baseline, after_baseline);
    sessionArr.push_back(session);

    string ID_Str = to_string(session->getID());
    string dateStr = session->getDateStr();

    // Convert the data strings to QStrings
    QString qID_Str = QString::fromStdString(ID_Str);
    QString qDateStr = QString::fromStdString(dateStr);
    string data = session->getData();
    QString record = QString::fromStdString(data);
    QList<QStandardItem*> rowItems;
    rowItems.append(new QStandardItem(record));
    logModel->appendRow(rowItems);
}

void Log::addSession(SessionData * session){
    sessionArr.push_back(session);
}

// only display date and time from sessions
void Log::printSession(){
    for(auto el=begin(sessionArr); el!= end(sessionArr); ++el){
                cout<<"ID: " <<(*el)->getID() <<", Date: " << (*el)->getDateStr()<<endl;
     }

}

// display date, time and baseline frequencies
void Log::printToPC(){
    for(auto el=begin(sessionArr); el!= end(sessionArr); ++el){
                cout<<"ID: " <<(*el)->getID() << endl;
                cout<<"Date: " << (*el)->getDateStr()<<endl;
                cout<<"Before_Baseline " << (*el)->getBeforeBaseline()<<endl;
                cout<<"After_Baseline " << (*el)->getAfterBaseline()<<endl;

     }
}


void Log::setListView(QListView *list){
    logView = list;
    logView ->setModel(logModel);
}
