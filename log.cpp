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

/**
 * @brief Log::setupHeader
 * Set up the header for the log
 */
void Log::setupHeader(){

    // Header Values to be displayed in SEssion LOg
    string header = "ID     DATE";
    QString dateHeader = QString::fromStdString(header);


    // Add a row to the model
    QList<QStandardItem*> rowItems;
    rowItems.append(new QStandardItem(dateHeader)); // Add the date string as the first column
    logModel->appendRow(rowItems);
}

/**
 * @brief Log::addSession
 * @param id int id of the session
 * @param now time_t end time of the session
 * @param before_baseline float before baseline frequency
 * @param after_baseline float after baseline frequency
 * Add a session to the log
 */
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

/**
 * @brief Log::addSession
 * @param session SessionData* session
 * Add a session to the log
 */
void Log::addSession(SessionData * session){
    sessionArr.push_back(session);
}

/**
 * @brief Log::printSession
 * print only the date and time from sessions for the log Menu
 */
void Log::printSession(){
    for(auto el=begin(sessionArr); el!= end(sessionArr); ++el){
                cout<<"ID: " <<(*el)->getID() <<", Date: " << (*el)->getDateStr()<<endl;
     }

}

/**
 * @brief Log::printToPC
 * print the date, time and baseline frequencies to the PC (console)
 */
void Log::printToPC(){
    cout<<endl<<"--- Session Log Data ---"<<endl;
    for(auto el=begin(sessionArr); el!= end(sessionArr); ++el){
                cout<<"ID: " <<(*el)->getID() << endl;
                cout<<"Date: " << (*el)->getDateStr()<<endl;
                cout<<"Before_Baseline " << setprecision(5) << (*el)->getBeforeBaseline()<<endl;
                cout<<"After_Baseline " << setprecision(5) << (*el)->getAfterBaseline()<<endl<<endl;

     }
}

/**
 * @brief Log::setListView
 * @param list QListView* list
 * Set the list view for the log
 */
void Log::setListView(QListView *list){
    logView = list;
    logView ->setModel(logModel);
}
