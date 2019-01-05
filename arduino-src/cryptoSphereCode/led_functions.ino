void getColor(int * buff, int n, boolean fromVar) {
  int color = 0;
  if (fromVar) {
    color = ledColors[n];
  } else {
    color = strip.getPixelColor(n);
  }
  int red = color >> 16;
  int green = (color >> 8) - (red << 8);
  int blue = color - (red << 16) - (green << 8);
  buff[0] = red;
  buff[1] = green;
  buff[2] = blue;
}

void getColor(int * buff, int n) {
  getColor(buff, n, false);
}

void getColor(int * buff) {
  getColor(buff, highlighted, false);
}

int getDimmedColor(int color, int percentage) {
  int red = color >> 16;
  int green = (color >> 8) - (red << 8);
  int blue = color - (red << 16) - (green << 8);

  int remaining_percentage = 100 - percentage;
  remaining_percentage = (int) (pow(remaining_percentage, 0.6) / pow(100.0, 0.6) * 100.0);
  percentage = 100 - remaining_percentage;
  return strip.Color(red * percentage / 100,
                     green * percentage / 100,
                     blue * percentage / 100);
}

int getDimmedColor(char * colorStr, int percentage) {
  return getDimmedColor(color(colorStr), percentage);
}

void updateLed(int led, uint32_t color, boolean show) {
  if (led >= 0 && led < NUM_LEDS){
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


void longShakeDimming(int start) {
  if (movementDuration > start) {
    int percentage = (int) ((longShakeDuration - start) - (movementDuration - start)) / ((float) longShakeDuration - start) * 100.0;
    for (int led = 0; led < NUM_LEDS; led++) {
      int color = ledColors[led];
      updateLed(led, getDimmedColor(color, percentage), false);
      
    }
  }
}



void binaryFeedbackAnimation(int type) {
        blinkHighlighted(-1, 100, strip.Color(0, 150, 0), strip.Color(150, 0, 0));
        if (movementDuration > longShakeDuration / 3) {
          longShakeDimming(longShakeDuration / 3);
        }
}

void colorFeedbackAnimation(int type) {
    blinkHighlighted(-1, 100, -1);
    if (movementDuration > longShakeDuration / 3) {
      updateLedRange(highlighted, NUM_LEDS-1, strip.Color(0, 0, 0), false);
      longShakeDimming(longShakeDuration / 3);
    }
}

void restoreLedsFromState() {
  for (int led = 0; led < NUM_LEDS; led++) {
    int color = ledColors[led];
    updateLed(led, color, false);
  }
}
int lastLevel = -1;
void displayEncryptionLevel() {
  if (encryptionLevel != lastLevel) {
    Serial.println(encryptionLevel);
    updateAllLeds(0, false);
    updateLedRange(0, encryptionLevel, strip.Color(0,150,0));
    lastLevel = encryptionLevel;
  } else {
    float span = nextThres - lastThres;
    float accelIntoSpan = getAcceleration() - lastThres;
    int percentage = (int) (accelIntoSpan / span * 100.0);
    updateLed(encryptionLevel+1, getDimmedColor(strip.Color(150,50,0), percentage));
  }
}

int highlightdimPercentage = 100;
void displayHighlighted() {
  int percentage = (millis() - highlightTime) / 10 % (highlightdimPercentage * 2);
  if (percentage < highlightdimPercentage) {
    percentage = 100 - percentage;
    updateLed(highlighted, getDimmedColor(highlightedColor, percentage), false);
  }
  else {
    percentage -= ((2 * highlightdimPercentage) - 100);
    updateLed(highlighted, getDimmedColor(highlightedColor, percentage), false);
  }
}

void highlight(int direction, boolean absolute) {
  if (highlightedColor != 0) {
    updateLed(highlighted, highlightedColor);
  }

  if (absolute) {
    highlighted = direction % NUM_LEDS;
  }
  else {
    highlighted = (highlighted + direction + NUM_LEDS) % NUM_LEDS;
  }

  highlightedColor = strip.getPixelColor(highlighted);
  while (highlightedColor == 0) {
    highlighted = (highlighted + direction + NUM_LEDS) % NUM_LEDS;
    highlightedColor = strip.getPixelColor(highlighted);
  }
  highlightTime = millis();
}

void highlight(int direction) {
  highlight(direction, false);
}

int lastRotToColor = 0;
void rotationToColor() {
//  if (movement) {
//    return; //This means that detectMovement is in the process of registrering movement.
//  }

  double gx = imu.calcGyro(imu.gx / 20);
  double gy = imu.calcGyro(imu.gy / 20);
  double gz = imu.calcGyro(imu.gz / 20);
  
  if (millis() - lastRotToColor > 1) {
    lastRotToColor = millis();
    int red = highlightedColor >> 16;
    int green = (highlightedColor >> 8) - (red << 8);
    int blue = highlightedColor - (red << 16) - (green << 8);

    double new_red = (float) red;
    double new_green = (float) green;
    double new_blue = (float) blue;
    
    if (gx > 0.0) {
      new_red = new_red + gx;
    } else {
      new_red = new_red + gx / 2.0;
      new_green = new_green - gx / 2.0;
      new_blue = new_blue - gx / 2.0;
    }

    if (gy > 0.0) {
      new_green = new_green + gy;
    } else {
      new_red = new_red - gy / 2.0;
      new_green = new_green + gy / 2.0;
      new_blue = new_blue - gy / 2.0;
    }

    if (gz > 0.0) {
      new_blue = new_blue + gz;
    } else {
      new_red = new_red - gz / 2.0;
      new_green = new_green - gz / 2.0;
      new_blue = new_blue + gz / 2.0;
    }

    new_red = std::max(0.0, std::min(255.0, new_red));
    new_green = std::max(0.0, std::min(255.0, new_green));
    new_blue = std::max(0.0, std::min(255.0, new_blue));
    

    double intensity = sqrt(sq(new_red) + sq(new_green) + sq(new_blue));
    red = (int) (new_red / intensity * 150.0);
    green = (int) (new_green / intensity * 150.0);
    blue = (int) (new_blue / intensity * 150.0);
    
    highlightedColor = (red << 16) + (green << 8) + blue;
    updateLed(highlighted, highlightedColor, false);
    //highlightedColor = strip.getPixelColor(highlighted);
  }
}

int color(const char * name) {
  if( name == "white") return strip.Color(100, 100, 100);
  else if( name == "red") return strip.Color(150, 0, 0);
  else if( name == "green") return strip.Color(0, 150, 0);
  else if( name == "blue") return strip.Color(0, 0, 150);
  else if( name == "yellow") return strip.Color(100, 100, 0);
  else if( name == "purple") return strip.Color(100, 0, 100);
  else if( name == "cyan") return strip.Color(0, 100, 100);
  else if( name == "orange") return strip.Color(130, 80, 0);
  else if( name == "maroon") return strip.Color(150, 30, 100);
  else if( name == "pink") return strip.Color(130, 100, 105);
  return strip.Color(0, 0, 0); // Black / off
}

void resetLeds() {
  for (int led = 0; led < NUM_LEDS; led++) {
    ledStates[led] = 0;
    ledColors[led] = 0;
    flipStates[led] = 1;
    for (int led_ = 0; led_ < NUM_LEDS; led_++) {
      logic[led][led_] = 0;
    }
  }
  lastBlink = 0;
  blinkState = 0;
  updateAllLeds(strip.Color(0,0,0), true);
  
}

