#ifndef SESSIONDATA_H
#define SESSIONDATA_H

#include <QObject>
#include <ctime>
#include <string>

using namespace std;

class SessionData
{
public:
    SessionData(int id, time_t time, float before_baseline,float after_baseline);

    // GETTERS

    int getID();
    int getBeforeBaseline();
    int getAfterBaseline();

    /**
     * @brief return the string version of the date from struct tm in the format dd/mm/yyyy
     *
     *
     * @param num takes a tm struct from ctime.h
     * @return string representation of the date (dd/mm/yyyy)
     *
     */
    string getDateStr();


    /**
     * @brief return the string version of the weekday from struct tm
     *
     *
     * @param num takes the integer representing the weekday in the struct tm
     * @return string representation of the day of the week
     *
     * @warning returns empty string if num is less than 0 or greater than 6
     */
    string getWeekdayStr(int num);

    string getData();



private:
    int id;
    tm * session_time;
    float before_baseline;
    float after_baseline;
    string day;
};

#endif // SESSIONDATA_H
