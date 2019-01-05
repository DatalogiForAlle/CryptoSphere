#ifndef CircleLeds_h
#define CircleLeds_h

#include "Arduino.h"

class CircleLeds
{
  private:
    int value;
    int numLeds;

  public:
    CircleLeds();
    CircleLeds(int num);
    int next ();
    int previous ();
    int set (int newValue);
    int get ();
};

#endif
