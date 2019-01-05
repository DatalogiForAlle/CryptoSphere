// In this section we use both the simple "delay()" functio  and the more advance DelayDiku class.
// Premade functions must use the designated type of delays, otherwise yourcryptoSphere may stop working correctly.
// The essential difference between the two types of delay is that "delay()" will halt the entire program for the duration,
// while DikuDelay will let the program do other stuff while waiting for the delay.
// E.g. displaying an animation to indicate that the CryptoSphere is processing something,
// would require that you use DikuDelay, as otherwise it wouldn't be able to perform the processing...


CircleLeds currentLed = CircleLeds(NUM_LEDS);
DelayDiku delay1;

// This animation is played while the CryptoSphere is working on something,
// or waiting for a response to a server-request.
// NOTE: Must use DelayDiku
void processingAnimation() {
  if (delay1.check()) { //Check if delay has passed
    updateLed(currentLed.get(), 0); // Set the current LED to 0 (Black / off)
    updateLed(currentLed.next(), color("cyan")); // Set the next LED to red
    delay1.set(50); // Let the delay be passed after 50 ms
  }
}


// This animation is played upon a successfully sent or decrypted message
// NOTE: Must use normal delay
void confirmedAnimation(int num_blinks) {
  blinkAll(num_blinks, 100, strip.Color(0,150,0));
}

// This animation is played upon an unsuccessfully sent or decrypted message
// NOTE: Must use normal delay
void refusedAnimation(int num_blinks) {
  blinkAll(num_blinks, 100, strip.Color(150,0,0));
}


// "waitingAnimation" is used when the CryptoSphere is in a passive state. E.g. waiting for you to send smoething to it.
// NOTE: It must use DikuDelay
DelayDiku waitingDelay;
int waitingCounter = 0;

void waitingAnimation(int color) {
  if (waitingDelay.check()) { //Check if delay has passed

    waitingCounter = (waitingCounter + 1) % 200; // This make it go 0, 1, 2, ... 198, 199, 0, 1, 2 ...
  
    int percentage = abs(waitingCounter - 100); // Since (waitingCounter - 100) will be in the range -100 to 99,
    // "percentage" will then go 100, 99, 98 ... 1, 0, 1, 2 ... 99, 100, 99 ....... 

    // Then we get the dimmed color
    int dimmedColor = getDimmedColor(color, percentage);

    // Set all the leds to the new dimmed color
    updateAllLeds(dimmedColor);
    
    waitingDelay.set(10); // Let the delay be passed after only 10 ms
  }
}







// The following functions are convenient for performing easy blinking. Use num_blinks = 0 if you want it to use the advanced delay function.


int lastBlink = 0;
int blinkState = 0;
void blinkHighlighted(int num_blinks, int delay_time, int color1, int color2) {
  if (color1 < 0) {
    color1 = highlightedColor;
    color2 = 0;
  }
  
  blinkLed(highlighted, num_blinks, delay_time, color1, color2);
}

void blinkHighlighted(int num_blinks, int delay_time, int color) {
  blinkHighlighted(num_blinks, delay_time, color, 0);
}

void blinkHighlighted(int num_blinks, int delay_time) {
  blinkHighlighted(num_blinks, delay_time, -1, 0);
}

void blinkLed(int led, int num_blinks, int delay_time, int color1, int color2) {
  if (color1 < 0) {
    color1 = strip.Color(150, 0, 0); // Default red blinking
    color2 = 0;
  }
  
  if (num_blinks < 1 && millis()-lastBlink > delay_time) {
    lastBlink = millis();
    if (blinkState == 0) {
      updateLed(led, color1, false);
      blinkState = 1;
    } else {
      updateLed(led, color2, false);
      blinkState = 0;
    }
  } else if (num_blinks > 0) {
    for (int n = 0; n < num_blinks; n++) {
      updateAllLeds(color1);
      delay(delay_time);
      updateAllLeds(color2);
      delay(delay_time);
    }
  }
}

void blinkLed(int led, int num_blinks, int delay_time, int color) {
  blinkLed(led, num_blinks, delay_time, color, 0);
}

void blinkAll(int num_blinks, int delay_time, int color) {
  for (int n = 0; n < num_blinks; n++) {
    updateAllLeds(color);
    delay(delay_time);
    updateAllLeds(0);
    delay(delay_time);
  }
}

