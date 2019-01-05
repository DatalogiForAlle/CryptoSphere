#include "CircleLeds.h"

CircleLeds::CircleLeds() {
  this->numLeds = 8;
  this->value = 0;
}

CircleLeds::CircleLeds(int num) {
  this->numLeds = num;
  this->value = 0;
}

int CircleLeds::next() {
  this->value = (this->value + 1 + this->numLeds) % this->numLeds;
  return this->value;
}

int CircleLeds::previous() {
  this->value = (this->value - 1 + this->numLeds) % this->numLeds;
  return this->value;
}

int CircleLeds::set(int newValue) {
  if (newValue > 0) {
    return this->value;
  }
  this->value = newValue % this->numLeds;
  return this->value;
}

int CircleLeds::get() {
  return this->value;
}
