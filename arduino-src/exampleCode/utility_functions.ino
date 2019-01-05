void updateLed(int led, uint32_t color, boolean show) {
  if (led >= 0 && led < NUM_LEDS) {
    strip.setPixelColor(led, color);
  }
  if (show) {
    strip.show();
  }
}

void updateLed(int led, uint32_t color) {
  updateLed(led, color, true);
}

void updateLedRange(int a, int b, uint32_t color, boolean show) {
  if (b > NUM_LEDS - 1) {
    b = NUM_LEDS - 1;
  }
  for (int n = a; n <= b; n++) {
    strip.setPixelColor(n, color);
  }
  if (show) {
    strip.show();
  }
}

void updateLedRange(int a, int b, uint32_t color) {
  updateLedRange(a, b, color, true);
}

void updateAllLeds(uint32_t color, boolean show) {
  for (int n = 0; n < NUM_LEDS; n++) {
    strip.setPixelColor(n, color);
  }
  if (show) {
    strip.show();
  }
}

void updateAllLeds(uint32_t color) {
  updateAllLeds(color, true);
}

int getDimmedColor(int color, int percentage) {
  int red = color >> 16;
  int green = (color >> 8) - (red << 8);
  int blue = color - (red << 16) - (green << 8);

  int remaining_percentage = 100 - percentage;
  remaining_percentage = (int) (pow(remaining_percentage, 0.5) / pow(100.0, 0.5) * 100.0);
  percentage = 100 - remaining_percentage;
  return strip.Color(red * percentage / 100,
                     green * percentage / 100,
                     blue * percentage / 100);
}

int color(const char * name) { // strip.Color values go from 0 to 255!
  if( name == "white") return strip.Color(100, 100, 100); // The format is Red, Green, Blue values
  else if( name == "red") return strip.Color(150, 0, 0);
  else if( name == "green") return strip.Color(0, 150, 0);
  else if( name == "blue") return strip.Color(0, 0, 150);
  else if( name == "yellow") return strip.Color(100, 100, 0);
  else if( name == "purple") return strip.Color(100, 0, 100);
  else if( name == "cyan") return strip.Color(0, 100, 100);
  else if( name == "orange") return strip.Color(130, 80, 0);
  return strip.Color(0, 0, 0); // Black / off
}

int getDimmedColor(char * colorStr, int percentage) {
  return getDimmedColor(color(colorStr), percentage);
}

void accelerometerLoop() {
  if (! imu.accelAvailable() ) {
    Serial.println("No connection to accelerometer");
    imu.begin();
    
  }

  imu.readGyro();
  imu.readAccel();
  imu.readMag();

  ax = imu.calcAccel(imu.ax);
  ay = imu.calcAccel(imu.ay);
  az = imu.calcAccel(imu.az);
  gx = imu.calcGyro(imu.gx);
  gy = imu.calcGyro(imu.gy);
  gz = imu.calcGyro(imu.gz);
}

void formatAndPrintAccelerometerData() {
    char axstr[15];
    char aystr[15];
    char azstr[15];
    char gxstr[15];
    char gystr[15];
    char gzstr[15];
    dtostrf(ax, 4, 2, axstr);
    dtostrf(ay, 4, 2, aystr);
    dtostrf(az, 4, 2, azstr);
    dtostrf(gx, 4, 2, gxstr);
    dtostrf(gy, 4, 2, gystr);
    dtostrf(gz, 4, 2, gzstr);
    Serial.printf("Accel: x=%s, y=%s, z=%s || Gyro: x=%s, y=%s, z=%s",
                  axstr, aystr, azstr, gxstr, gystr, gzstr);
    Serial.println();
}

DelayDiku padDelay;
void printAccelerometerData(int delay) {
  if (delay <= 0) {
    formatAndPrintAccelerometerData();
  } else {
    if (padDelay.check()) {
      formatAndPrintAccelerometerData();
      padDelay.set(delay);
    }
  }
}

void printAccelerometerData() {
  printAccelerometerData(0);
}
