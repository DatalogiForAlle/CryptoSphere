#include "DelayDiku.h"

DelayDiku::DelayDiku() {
  this->target = 0;
  this->running = false;
  this->first = true;
  this->started = -1;
}

void DelayDiku::set(int time) {
  if (!running) {
    this->started = millis();
    this->target = this->started+time;
    this->running = true;
    this->first = false;
  }
}

bool DelayDiku::check() {
  if (millis() >= this->target && this->running || this->first) {
    this->target = 0;
    this->running = false;
    this->first = false;
    this->started = -1;
    return true;
  }
  return false;
}

int DelayDiku::getTarget() {
  return this->target;
}

int DelayDiku::timeLeft() {
  return this->target - millis();
}

int DelayDiku::percentage() {
  if (this->target < millis()) {
    return 100;
  }
  return (int) (100.0 - ((this->target - millis()) / ((float)(this->target - this->started)) * 100.0));
}
