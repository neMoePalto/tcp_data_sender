#ifndef AVERAGETIME_H
#define AVERAGETIME_H
#include <vector>

class AverageTime
{
public:
    AverageTime();
    void fixStartTime();
    void fixEndTime();
    double getAverage();
private:
    struct PairOfValues
    {
        long a{};
        long b{};
    };
    std::vector<PairOfValues> _vec{};
    PairOfValues _currentPair{};
    long getMicrosecondsTime();
};

#endif // AVERAGETIME_H
