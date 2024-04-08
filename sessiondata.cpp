#include "sessiondata.h"

using namespace std;

SessionData::SessionData(int id, tm *time, float baseline_before, float baseline_after)
{
  this-> id = id;
  session_time = time;
  before_baseline = baseline_before;
  after_baseline = baseline_after;
  day = getWeekdayStr(session_time->tm_wday);
}

// function to return the string version of the weekday from struct tm
string SessionData::getWeekdayStr(int num){

    if (num < 0 || num > 6) {
        return "";
    } else {
        string days[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
        return days[num];
    }
}

string SessionData::getDateStr(){
    return to_string(this->session_time->tm_mday) + "/"
            + to_string(this->session_time->tm_mon) + "/"
            +to_string(this->session_time->tm_year);

}

string SessionData::getData(){
    return to_string(getID()) + "      " + getDateStr() + "\n";
}

int SessionData::getID(){
    return this->id;
}

int SessionData::getBeforeBaseline(){
    return this->before_baseline;

}

int SessionData::getAfterBaseline(){
    return this->after_baseline;
}
