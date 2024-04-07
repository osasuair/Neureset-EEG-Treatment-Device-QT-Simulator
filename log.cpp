#include "log.h"

using namespace std;

Log::Log(QObject *parent)
    : QObject{parent}
{

}


Log::~Log(){

    int len = sessionArr.size();
    for (int i   = 0; i < len ;  i++ ){
        delete sessionArr[i];
    }
}

// buch of sessiondata pointers

void Log::addSession (int id, tm* now, float before_baseline, float after_baseline){
    SessionData *session = new SessionData(id, now, before_baseline, after_baseline);
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
                cout<<"Before_Baseline " << (*el)->getInitialBaseline()<<endl;
                cout<<"After_Baseline " << (*el)->getAfterBaseline()<<endl;

     }
}
