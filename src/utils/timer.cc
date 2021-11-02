#pragma once

#include "timer.hh"

Timer::Timer(duration) {
  now = std::chrono::system_clock::now();
  this->duration = std::chrono::milliseconds(duration);
  this->endTime = now + this->duration;
}

bool Timer::isOver() {
  now = std::chrono::system_clock::now()
  return now > this->endTime;
}

void Timer::reset() {
  now = std::chrono::system_clock::now()
  this->endTime = now + this->duration;
}

