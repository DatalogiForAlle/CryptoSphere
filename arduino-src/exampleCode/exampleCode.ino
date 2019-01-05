#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>
#include "constants_and_globals.h"
#include "CircleLeds.h"
#include "DelayDiku.h"



#define STRIP_PIN  5    // Digital IO pin connected to the NeoPixels.
#define NUM_LEDS   8

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

//Dont mind this...
CircleLeds currentLed = CircleLeds(NUM_LEDS);
DelayDiku delay1, delay2;
// ----------------


void setup() {

  // put your setup code here, to run once:
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(128); // Half brightness. 0 to 255
  strip.show(); // Initialize all pixels to 'off'
  imu.begin();
  currentLed.previous();
}


void loop() {
  //This just loads the values from the accelerometer
  //accelerometerLoop();
  
  // And this just writes them out in the serial monitor every 0.5 seconds.
  //printAccelerometerData(500);

  ledLoop();
  //waveLoop();
  //waveLoopNicer();
  //pulseLoop();
  //axisToLed();
  //movementToLed(2.5);
  //rotationSwitchLed(250);

}


void ledLoop() {
    delay(100); //Set a 100 milliseconds delay
    updateLed(currentLed.get(), 0); // Set the current LED to 0 (Black / off)
    updateLed(currentLed.next(), color("green")); // Set the next LED to red
    
  // Try experimenting with updateAllLeds(<color>)!
  // Also note, that you can create custom colors
  //   using strip.Color(<red_value>, <green_value>, <blue-value>) (values from 0 to 255).
  // You can  define new color names in the color function youself!
  // And if you are brave, try dimming the colors,
  //   using getDimmedColor(<color>, (int) <percentage>)
  //   to create cool pulsing effects!
}

// This function is the exact same as the above, except that it uses the more advanced "DikuDelay" class.
// An explaination for why this may be necessary sometimes comes later.
void ledLoop_advancedDelays() {
  if (delay1.check()) { //Check if delay has passed
    
    updateLed(currentLed.get(), 0); // Set the current LED to 0 (Black / off)
    
    updateLed(currentLed.next(), color("green")); // Set the next LED to red
    
    delay1.set(200); // Let the delay be passed after 200 ms
  }
}


// This function display a pulsing color.
int n = 0; // A simple counter;
void pulseLoop() {
  if (delay1.check()) { //Check if delay has passed

    n = (n + 1) % 200; // This make n go 0, 1, 2, ... 198, 199, 0, 1, 2 ...
  
    int percentage = abs(n - 100); // This function "abs" (absolute) basically removes any "-" sign.
    // So, since (n - 100) will be in the range -100 to 99,
    // "percentage" will then go 100, 99, 98 ... 1, 0, 1, 2 ... 99, 100, 99 ....... 

    // Then we get the dimmed color
    int dimmedColor = getDimmedColor(color("green"), percentage);

    // Set all the leds to the new dimmed color
    updateAllLeds(dimmedColor);
    
    delay1.set(10); // Let the delay be passed after only 10 ms
  }
}


// This function produces a simple wave animation.
void waveLoop() {
    currentLed.next(); // Shifts the counter by 1
    updateLed(currentLed.next(), getDimmedColor(color("cyan"), 0)); // Set the next LED to cyan (0%), so just "off/black"
    updateLed(currentLed.next(), getDimmedColor(color("cyan"), 14)); // Set the next LED to a dim cyan
    updateLed(currentLed.next(), getDimmedColor(color("cyan"), 29)); // Set the next LED to a slighty less dim cyan
    updateLed(currentLed.next(), getDimmedColor(color("cyan"), 43)); //...
    updateLed(currentLed.next(), getDimmedColor(color("cyan"), 57)); //...
    updateLed(currentLed.next(), getDimmedColor(color("cyan"), 71)); //...
    updateLed(currentLed.next(), getDimmedColor(color("cyan"), 86)); //...
    updateLed(currentLed.next(), getDimmedColor(color("cyan"), 100)); // Set the next LED to full cyan
    delay(75);
}


//This function does the excact same as the above one, but using nicer coding.
void waveLoopNicer() {
  currentLed.next(); // Shifts the counter by 1
  for (int n = 0; n < NUM_LEDS; n++) { // This is a for loop. It loops as long as n is less than NUM_LEDS
    int percentage = (int) (100.0 / (NUM_LEDS - 1) * n); // Calculate percentage
    updateLed(currentLed.next(), getDimmedColor(color("cyan"), percentage));
  }
  delay(75);
  // You could let this function use DikuDelays instead, and then you can use it for a "processing animation"
  // (Explained later)
}


void axisToLed() {
  int leds = NUM_LEDS * ax; 
  // So what happens here is pretty simple actually:
  // "ax" is the value of force along the x-axis. If we assume that the device
  // is not being shaken, then "ax" will probably be somewhere between 0 and 1.
  // This is due to the force of gravity!
  // A value of about 1 more or less means the force of gravity (9.82 m/s^2).
  // So by multiplying NUM_LEDS by a value between 0 and 1,
  // We should get the number of leds illustrating the approximate value of "ax",
  // That is, how much gravity is pulling on that axis.

  // The value false, here just means that we do not update the physical leds quite yet.
  updateAllLeds(color("off"), false); // Then we turn off all the leds.
  
  // And then we overwite the black leds ranging from  0 to "leds" with red.
  updateLedRange(0, leds, color("red")); // All updates are now shown.
 
}


// So you probably know the Pythagorean theorem, right? (  sqrt( a^2 + b^2 ) = c )
// Well, as a matter of fact, that holds for 3 dimensions as well;
// So, in this case, with forces along x, y and z axes, we have:
// sqrt( x^2 + y^2 + z^2 ) = f, where f is the size of the force
// Might not be that intuitive (Unless you know vector mathematics),
// but consider f to be the force of the movement.
// Thus we have:
float movementForce() {
  return fabs(sqrt(sq(ax) + sq(ay) + sq(az)) - 1.0 );
  // But wait! What is this "- 1" and "fabs" for??
  // Well, as I mentioned, the gravitational pull equates to about 1.0 on the accelerometers,
  // and we want to ignore this, thus we subtract 1.0 from the measured force.
  // The "fabs" ensures that if the result is negative,
  // then we get the equivalent positive value.
  // e.g f = 0.65 <=> f - 1 = -0.35 <=> fabs(f - 1) = 0.35 
}


// More or less the same principles apply for the rotational force.
float rotationForce() {
  return sqrt(sq(gx) + sq(gy) + sq(gz));
}


// This function would display the amount of force by using the LEDS
void movementToLed(float maximumForce) {
  // Here we calculate the fraction of the measuredForce by the maximumForce that we have defined.
  float fraction = movementForce() / maximumForce; 
  
  // And here we calculate the number of LEDs to turn on.
  int leds = round(NUM_LEDS * fraction);

  // Then we turn off all LEDS
  updateAllLeds(color("black"));

  // And then we turn on the new amount of LEDs 
  updateLedRange(0, leds - 1, color("red"));
  // We subtract 1 from "leds" because we want a force of 0 to have all LEDs turned off.
  // Thus, we would be using "updateLedRange(0, -1, ...)" which doesn't show any leds.
  // updateLedRange(0, 0, ...) would turn on the first LED ("From led #0 to led #0")
}


// This function should switch the LED when the accelerometer is rotated
void rotationSwitchLed(float threshold) {
  if (rotationForce() > threshold) { // Check if the rotational force is above the defined threshold
    updateLed(currentLed.get(), 0); // Set the current LED to 0 (Black / off)
    updateLed(currentLed.next(), color("green")); // Set the next LED to green
    delay(500); // Wait 500 ms before we check again.
  }
}

