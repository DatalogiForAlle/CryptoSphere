// VERSION 1.0.0
#include "settings.h"
#include "constants_and_globals.h"

void setup() {
  baseSetup();
}


// If you have some global variables, put them here

// --^

void loop() {
  websocket.loop();
  accelerometerLoop();

  switch(loopState) {
    case WAITING:
      waitingAnimation(color("orange"));
      break;
    case WAITINGFORCONFIRMATION:
      processingAnimation();
      break;
    case SENDBINARY:
      sendBinaryPuzzleLooping();
      break;
    case RECEIVEBINARY:
      receiveBinaryPuzzleLooping();
      break;
    case RECEIVECOLOR:
      {
        boolean done = colorPuzzleLooping();
        if (done) {
          submitColorSolution();
        }
      }
      break;
    case SENDCOLOR:
      {
        boolean done = colorPuzzleLooping();
        if (done) {
          boolean success = storeMessage();
          if (success) {
            confirmedAnimation(3);
          } else {
            refusedAnimation(3);
          }
        }
      }
      break;
    default:
      break;
      
  }
}


