#pragma once

#include <chrono>
#include <ctime>
#include <cmath>

class Timer {

  public:
    Timer();

    bool isOver();
    void reset();

  private:
    std::chrono::milliseconds duration;
    std::chrono::time_point<std::chrono::system_clock> EndTime;
};
