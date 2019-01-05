#ifndef DelayDiku_h
#define DelayDiku_h

#include "Arduino.h"

class DelayDiku
{
  private:
    int target;
    bool running;
    bool first;
    int started;

  public:
    DelayDiku();
    int getTarget();
    void set(int time);
    bool check();
    int timeLeft();
    int percentage();
};

#endif
