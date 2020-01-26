#include "averagetime.h"
#include <sys/time.h>
#include <QDebug>

AverageTime::AverageTime()
{}

void AverageTime::fixStartTime()
{
    if (_currentPair.a == 0)
        _currentPair.a = getMicrosecondsTime();
}

void AverageTime::fixEndTime()
{
    _currentPair.b = getMicrosecondsTime();
    if (_currentPair.a != 0)
        _vec.push_back(_currentPair);

    _currentPair = {0, 0};
}

long AverageTime::getMicrosecondsTime()
{
    struct timeval currentTime;
    gettimeofday(&currentTime, nullptr);
    return currentTime.tv_sec * 1000000 + currentTime.tv_usec;
}

double AverageTime::getAverage()
{
    if (_vec.empty())
        return 0; // Не хватает исходных данных для проведения вычислений
    long average = 0;
    for (const auto &pair : _vec)
    {
        auto delta = pair.b - pair.a;
//        qDebug() << pair.a << ", " << pair.b  << ", " << delta;
//        qDebug() << delta;
        average = average + delta;
    }
    average = average / static_cast<long>(_vec.size());
    _vec.clear();
    return average * 1e-6;
}

