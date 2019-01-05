
bool connectToServer(const char * host, const int port) {
  if(!client.connect(host, port)) {
    Serial.println("Connection to server failed");
    return false;
  }
  return true;
}

bool connectToServer() {
  connectToServer(host, port);
}

bool connectToFB() {
  if(!fbClient.connect("graph.facebook.com", 443)) {
    Serial.println("Connection to FB failed");
    return false;
  }
  return true;
}

String request(String url, String content, String type, bool async)
{
  if (!connectToServer()) {
    return "";
  }
  String requestString = type + " " + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n";
  requestString += "Connection: close\r\n";
  if (type == "POST") {
    requestString += "Content-Type: application/json\r\n";
    requestString += "Content-Length: ";
    requestString += content.length();
    requestString += "\r\n\r\n" + content + "\r\n\r\n";
    
  }

  boolean retval = false;

  Serial.println(requestString);
  client.print(requestString);
  
  if (async) {
    return "";
  }
  else {
    unsigned long startTime = millis();
    while(client.available() == 0) {
      processingAnimation();
      if (millis() - startTime > 10000) {
        Serial.println("Client Timeout !");
        client.stop();
        return "";
      }
    }

    //DEBUGGING
    bool isBody = false;
    String body = "";
    while (client.connected())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        if (line == "\r" && !isBody) {
          Serial.print("<Empty line>");
          isBody = true;
        }
        else if (isBody) {
          body += line + "\r\n";
        }
        Serial.println(line);
      }
    }
    return body;
  }
  return "";
}

boolean validateFBToken(String token)
{

  if (!connectToFB()) {
      return false;
    }
    
  String requestString = "GET /me HTTP/1.1\r\n"
  "Host: graph.facebook.com\r\n"
  "Authorization: Bearer " + token + "\r\n"
  "Connection: close\r\n"
  "Cache-Control: no-cache\r\n\r\n";

  boolean retval = false;

  Serial.print(requestString);
  fbClient.print(requestString);
  unsigned long startTime = millis();
  while(fbClient.available() == 0) {
    processingAnimation();
    if (millis() - startTime > 10000) {
      Serial.println("Client Timeout !");
      fbClient.stop();
      return false;
    }
  }
  //DEBUGGING
  char until = '\n';
  while (fbClient.connected())
  {
    processingAnimation();
    if (fbClient.available())
    {
      String line = fbClient.readStringUntil(until);
      Serial.println(line);
      
      if (until == '}') {
        line += "}";
        DynamicJsonBuffer jsonBuffer(200);
        JsonObject& root = jsonBuffer.parseObject(line);
        if (root.containsKey("id") && root["id"] == userID) {
          accessToken = token;
          return true;
        }
      } else if (line == "\r") {
        Serial.print("<Empty Line>");
        until = '}';
      }
    }
  }
  return retval;
}

boolean storeMessage()
{
  if (!connectToServer()) {
    return false;
  }
  
  Serial.println("Storing message...");

  generate_garble(encrypted, msgLength);

  DynamicJsonBuffer jsonBuffer(2048);
  JsonObject& root = jsonBuffer.createObject();
  char * url = "/storeMessage.php";
  root["sender"] = userID;
  root["recipient"] = recipient;
  root["recipient_name"] = recipient_name;
  root["message"] = encrypted;
  root["message_intact"] = loadedMessage;
  if (loopState == SENDBINARY) {
    root["puzzle_type"] = "binary";
    root["puzzle_data"] = encryptionLevel;
  }
  else if (loopState == SENDCOLOR) {
    root["puzzle_type"] = "color";
    root["puzzle_data"] = getColorJson();
  }
  root["user_access_token"] = accessToken;

  char content[1024] = { 0 };
  root.printTo((char*)content, root.measureLength() + 1);
  String body = request(String(url), String(content), "POST", false);
  Serial.println("body: " + body);
  char json[255] = { 0 };
  body.toCharArray(json, 255);
  DynamicJsonBuffer jsonBuffer2(512);
  JsonObject& root2 = jsonBuffer2.parseObject(json);
  if (!root2.success()) {
    Serial.println("parseObject() failed");
    resetAll();
    return false;
  }
  
  if (root2.containsKey("type")) {
    websocket.sendTXT(wsNum, body);
    resetAll();
    return true;
  }
  
  resetAll();
  return false;
}

void generate_garble(char * buff, int length) {
  int n = 0;
  buff[n] = random(65, 91); // Uppercase letter
  for (n = 1; n < length; n++) {
    long r = random(5);
    if (r == 0 && buff[n-1] != ' ') {
      buff[n] = ' ';
    }
    else {
      buff[n] = random(97, 123); // Lowercase letter
    }
  }
  buff[n+1] = '\0';
}

char * getColorJson() {
  DynamicJsonBuffer jsonBuffer(400);
  JsonArray& root = jsonBuffer.createArray();
 
  for (int n = 0; n < highlighted; n++) {
    int color[3] = {0};
    getColor(color, n, true);
    JsonArray& colorJson = root.createNestedArray();
    colorJson.add(color[0]);
    colorJson.add(color[1]);
    colorJson.add(color[2]);
  }

  char json[200] = {0};
  root.printTo((char*)json, root.measureLength() + 1);

  return json;
}

String ipToString(IPAddress ip) {
  String strIP = String(ip[0]);
  for (int n = 1; n < 4; n++) {
    strIP += "." + String(ip[n]);
  }
  return strIP;
}

void handlePuzzle(char * json) {
  Serial.println(json);
  DynamicJsonBuffer jsonBuffer(1285);
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  // --------------------------------------------------------
  // BINARY PUZZLE
  // --------------------------------------------------------
  if (root["puzzle_type"] == "binary") {
    loopState = RECEIVEBINARY;
    Serial.println("start_state");
  
    for (int n = 0; n < NUM_LEDS; n++) {
      int state = (int) root["start_state"][n];
      if (state == 1) {
        ledColors[n] = strip.Color(0, 150, 0);
        updateLed(n, strip.Color(0, 150, 0));
      }
      else if (state == -1) {
        ledColors[n] = strip.Color(150, 0, 0);
        updateLed(n, strip.Color(150, 0, 0));
      } else {
        updateLed(n, strip.Color(0, 0, 0));
      }
      ledStates[n] = state;
      Serial.printf("%d ", state);
      flipStates[n] = 1;
    }
    Serial.println();
    strip.show();
  
    Serial.print("\n");
  
    for (int input = 0; input < NUM_LEDS; input++) {
      Serial.print("[ ");
      for (int led = 0; led < NUM_LEDS; led++) {
        int flip = (int) root["logic"][input][led];
        logic[input][led] = flip;
        Serial.printf("%d ", flip);
      }
      Serial.print("]\n");
    }
    highlightedColor = 0;
    highlight(0, true);
  }
  
  // --------------------------------------------------------
  // COLOR PUZZLE
  // --------------------------------------------------------
  else if (root["puzzle_type"] == "color") {
    loopState = RECEIVECOLOR;
  }
  
}

void switchPuzzleLed(int n) {
  for (int i = 0; i < NUM_LEDS; i++) {
//    ledStates = ledStatesRestore;
//    highlightTime = 0;
    int ledState = ledStates[i];
    Serial.print(ledState);
    if (ledState != 0) {
      ledState = ledState * logic[n][i];
      if (ledState == 1) {
        ledColors[i] = strip.Color(0, 150, 0);
        strip.setPixelColor(i, strip.Color(0, 150, 0));
      } else {
        ledColors[i] = strip.Color(150, 0, 0);
        strip.setPixelColor(i, strip.Color(150, 0, 0));
      }
  
      Serial.print(" -> ");
      Serial.print(ledState);
      
      ledStates[i] = ledState;
  
      Serial.print(" = ");
  
      Serial.println(ledStates[i]);
    }
    
  }
  strip.show();
  highlightedColor = strip.getPixelColor(highlighted);
  flipStates[n] = flipStates[n] * -1;
}

void submitSolution() {
  Serial.println("submitting soluton...");
  
  DynamicJsonBuffer jsonBuffer(200);;
  JsonObject& root = jsonBuffer.createObject();
  
  root["type"] = "solution";
  JsonArray& solution = root.createNestedArray("solution");
  
  for (int n = 0; n < NUM_LEDS; n++) {
    solution.add(flipStates[n]);
  }
  
  char solutionString[100] = {0};
  root.printTo((char*)solutionString, root.measureLength() + 1);

  websocket.sendTXT(wsNum, solutionString);
  loopState = WAITINGFORCONFIRMATION;
}

void submitColorSolution() {
  Serial.println("submitting soluton...");
  
  DynamicJsonBuffer jsonBuffer(400);
  JsonObject& root = jsonBuffer.createObject();
  
  root["type"] = "solution";
  JsonArray& solution = root.createNestedArray("solution");
  
  for (int n = 0; n < highlighted; n++) {
    int color[3] = {0};
    getColor(color, n, true);
    if (color[0] > 0 || color[1] > 0 || color[2] > 0) {
      JsonArray& colorJson = solution.createNestedArray();
      colorJson.add(color[0]);
      colorJson.add(color[1]);
      colorJson.add(color[2]);
    } else {
      break;
    }
  }

  char solutionString[200] = {0};
  root.printTo((char*)solutionString, root.measureLength() + 1);

  websocket.sendTXT(wsNum, solutionString);
  
  loopState = WAITINGFORCONFIRMATION;
}

int moveDelay = 1000;
int lastMoved = 0;
int rotationDelay = 1000;
int lastRotated = 0;
boolean movement = false;
int movementStart = 0;
int lastMovement = 0;
float accumForces[3] = {0, 0, 0};
float accumForce = 0;
float accumRotation = 0;
float accumRotations[3] = {0, 0, 0};
float rotationExtremes[3][2] = {{0, 0}, {0, 0}, {0, 0}};
int movementDuration = 0;
int measurements = 0;
float downVector[3] = {0, 0, 1.0};
boolean finishedMovement = false;
int type = 0;

float savedForce = 0;
float savedDuration = 0;
int savedMeasurements = 0;

int detectMovement(float rotationThreshold, float forceThreshold, boolean resetAccumulation) {
  type = 0;
  float ax = imu.calcAccel(imu.ax);
  float ay = imu.calcAccel(imu.ay);
  float az = imu.calcAccel(imu.az);
  float gx = imu.calcGyro(imu.gx);
  float gy = imu.calcGyro(imu.gy);
  float gz = imu.calcGyro(imu.gz);
  float force = fabs(sqrt(sq(ax) + sq(ay) + sq(az)) - 1 );
  float rotation = sqrt(sq(imu.calcGyro(imu.gx)) + sq(imu.calcGyro(imu.gy)) + sq(imu.calcGyro(imu.gz)));
  
  if ((rotation > rotationThreshold || force > forceThreshold)
       && !movement && lastMovement < millis() - movementTimeout) {
        
    movement = true;
    movementStart = millis();
    lastMovement = movementStart;

    if (resetAccumulation) {
      savedForce += accumForce;
      savedMeasurements += measurements;
      savedDuration += movementDuration;
      
      for(int i = 0; i < 3; i++) {
        accumForces[i] = 0;
        accumRotations[i] = 0;
        for (int j = 0; j < 2; j++) {
          rotationExtremes[i][j] = 0;
        }
      }
      accumForce = 0;
      accumRotation = 0;
      measurements = 0;
      
    }    
  }
  
  if (movement) {
    
    movementDuration = millis() - movementStart;
    
    if (rotation < rotationThreshold * 0.66
             && force < forceThreshold * 0.66
             && lastMovement < millis() - 400) {
              
      movement = false;
      
      Serial.print("Avg. Rot: ");
      float avgRotation = accumRotation / measurements;
      Serial.println(avgRotation);
  
      float avgForce = accumForce / measurements;
      Serial.print("Avg. Force: ");
      Serial.println(avgForce);
      float forceRotationRatio = avgForce / avgRotation;
      Serial.print("Ratio: ");
      Serial.println(forceRotationRatio, 4);
      if (avgForce > 0.40 && forceRotationRatio > 0.0020) {
        float dotDownAccum = accumForces[0] * downVector[0]
                              + accumForces[1] * downVector[1]
                              + accumForces[2] * downVector[2];
        float cosAngle = dotDownAccum / sqrt(sq(accumForces[0]) + sq(accumForces[1]) + sq(accumForces[2]));
        Serial.print("cosAngle: ");
        Serial.print(fabs(cosAngle), 4);
        if ((movementDuration) > longShakeDuration) {
          Serial.println("Long shake!");
          type = 2;
        } else {
          Serial.println("Shaken!");
          type = 1;
        }
      }
      else if (avgRotation > rotationAvgThreshold && forceRotationRatio < 0.0020 && movementDuration < 1500) {
        Serial.println("Twisted!");
        int max_pos = 0;
        int max_neg = 0;
        int rotationType = 0;
        for (int i = 1; i < 3; i++) {
          if (rotationExtremes[i][0] < rotationExtremes[i-1][0]) {
            max_neg = i;
          }
          if (rotationExtremes[i][1] > rotationExtremes[i-1][1]) {
            max_pos = i;
          }
        }

        if ((-rotationExtremes[max_neg][0]) > rotationExtremes[max_pos][1]) {
          rotationType = 10 * max_neg;
        } else {
          rotationType = 10 * max_pos + 1;
        }

        switch(rotationType) {
          case 0:
            {
              Serial.println("twisted x neg");
              type = 3;
            }
            break;
          case 1:
            {
              Serial.println("twisted x pos");
              type = 4;
            }
            break;
          case 10:
            {
              Serial.println("twisted y neg");
              type = 5;
            }
            break;
          case 11:
            {
              Serial.println("twisted y pos");
              type = 6;
            }
            break;
          case 20:
            {
              Serial.println("twisted z neg");
              type = 7;
            }
            break;
          case 21:
            {
              Serial.println("twisted z pos");
              type = 8;
            }
            break;
        }
      }
    } else {

      float avgRotation = accumRotation / measurements;
  
      float avgForce = accumForce / measurements;
      float forceRotationRatio = avgForce / avgRotation;
      
      if (avgForce > shakeAvgThreshold && forceRotationRatio > 0.0020) {
        float dotDownAccum = accumForces[0] * downVector[0]
                              + accumForces[1] * downVector[1]
                              + accumForces[2] * downVector[2];
        float cosAngle = dotDownAccum / sqrt(sq(accumForces[0]) + sq(accumForces[1]) + sq(accumForces[2]));
        if ((movementDuration) > longShakeDuration) {
          type = 20;
        } else {
          type = 10;
        }
      }

      
      if (rotation > rotationThreshold 
          || force > forceThreshold){
        lastMovement = millis();
      }
    } 
    accumForces[0] += ax;
    accumForces[1] += ay;
    accumForces[2] += az;
    accumRotations[0] += gx;
    accumRotations[1] += gy;
    accumRotations[2] += gz;
    rotationExtremes[0][0] = min(rotationExtremes[0][0], accumRotations[0]);
    rotationExtremes[0][1] = max(rotationExtremes[0][1], accumRotations[0]);
    rotationExtremes[1][0] = min(rotationExtremes[1][0], accumRotations[1]);
    rotationExtremes[1][1] = max(rotationExtremes[1][1], accumRotations[1]);
    rotationExtremes[2][0] = min(rotationExtremes[2][0], accumRotations[2]);
    rotationExtremes[2][1] = max(rotationExtremes[2][1], accumRotations[2]);
    accumRotation += rotation;
    accumForce += force;
    measurements += 1;
  }
  else if (lastMovement < millis() - 1000) {
                          
    downVector[0] = ax / (force + 1);
    downVector[1] = ay / (force + 1);
    downVector[2] = az / (force + 1);
//    Serial.print(downVector[0]);
//    Serial.print(", ");
//    Serial.print(downVector[1]);
//    Serial.print(", ");
//    Serial.print(downVector[2]);
//    Serial.println();
  
  }
  finishedMovement = false;
  if (type == 0 && force < minimalMovement && rotation < minimalRotation) {
    return -1;
  }
  else if (type > 0 && type < 10) {
    finishedMovement = true;
  }
  return type;
}

int detectMovement(boolean resetAccumulation) {
  return detectMovement(rotationStartThreshold, shakeStartThreshold, resetAccumulation);
}

int detectMovement() {
  return detectMovement(true);
}

float getAcceleration() {
  float value = (savedForce + accumForce) / (savedMeasurements + measurements) * (savedDuration + movementDuration) / 1000;
  if (value >= 0) {
    
  }
  else {
    value = 0.0;
  }
  return value;
}

int getXSign() {
  if (finishedMovement &&
      type >= 3 && type <= 8) {
    float pos = rotationExtremes[0][1];
    float neg = rotationExtremes[0][0];
    if (pos > (-neg) &&
        pos > 0.5 * rotationExtremes[1][1] &&
        pos > 0.5 * rotationExtremes[2][1]) {
      return 1;
    }
    else if (neg < (-pos) &&
        neg < 0.5 * rotationExtremes[1][0] &&
        neg < 0.5 * rotationExtremes[2][0]) {
      return -1;
    }
  }
  return 0;
}

int getYSign() {
  if (finishedMovement &&
      type >= 3 && type <= 8) {
    float pos = rotationExtremes[1][1];
    float neg = rotationExtremes[1][0];
    if (pos > (-neg) &&
        pos > 0.5 * rotationExtremes[0][1] &&
        pos > 0.5 * rotationExtremes[2][1]) {
      return 1;
    }
    else if (neg < (-pos) &&
        neg < 0.5 * rotationExtremes[0][0] &&
        neg < 0.5 * rotationExtremes[2][0]) {
      return -1;
    }
  }
  return 0;
}

int getZSign() {
  if (finishedMovement &&
      type >= 3 && type <= 8) {
    float pos = rotationExtremes[2][1];
    float neg = rotationExtremes[2][0];
    if (pos > (-neg) &&
        pos > 0.5 * rotationExtremes[0][1] &&
        pos > 0.5 * rotationExtremes[1][1]) {
      return 1;
    }
    else if (neg < (-pos) &&
        neg < 0.5 * rotationExtremes[0][1] &&
        neg < 0.5 * rotationExtremes[1][1]) {
      return -1;
    }
  }
  return 0;
}

void nextThreshold() {
  float thres = 0;
  for (int n = 0; n <= encryptionLevel; n++) {
    thres += pow(1.0, n); //1.4
  }
  nextThres = 5*thres;
}

int colorPuzzleLooping() {
  int type = detectMovement();
  rotationToColor();
  if (type == -1) {
    if (lastMinimalMovement > 0 && millis() - lastMinimalMovement > longShakeDuration) {
      Serial.println("Move led");
      updateLed(highlighted, highlightedColor, false);
      ledColors[highlighted] = highlightedColor;
      highlighted += 1;
      if (highlighted >= NUM_LEDS) {
        return 1;
      }
      lastMinimalMovement = millis();
    }
    else if (lastMinimalMovement > 0) {
      int percentage = (int) ((longShakeDuration - (millis() - lastMinimalMovement)) / ((float) longShakeDuration) * 100.0);
       updateLed(highlighted, getDimmedColor(highlightedColor, percentage), false);
    }
  }
  else if (type == 0) {
    lastMinimalMovement= millis();
  }
  else if (type == 1) {
    lastMinimalMovement = millis();
    updateLedRange(highlighted, NUM_LEDS-1, strip.Color(0, 0, 0), false);
    ledColors[highlighted] = 0;
    highlighted = max(0, highlighted - 1);
    restoreLedsFromState();
  }
  else if (type == 2 || type == 20) {
    lastMinimalMovement = millis();
    return 1;
  }
  else if (type == 10) {
    lastMinimalMovement = millis();
    colorFeedbackAnimation(type);
  }
  strip.show();

  return 0;
}

void sendBinaryPuzzleLooping() {
  int type = detectMovement(200, 2.0, true);
  float acceleration = getAcceleration();
  if (acceleration >= nextThres && encryptionLevel < NUM_LEDS) {
    Serial.print("oldNextThres: ");
    Serial.println(nextThres);
    encryptionLevel++;
    lastThres = nextThres;
    nextThreshold();
    Serial.print("newNextThres: ");
    Serial.println(nextThres);
  }
  displayEncryptionLevel();
  
  if (type >= 3 && type <= 8) {
    Serial.print("Acceleration: ");
    Serial.print(acceleration);
    Serial.println();
    Serial.printf("Writing message (%s) to server.\n", loadedMessage);
    Serial.printf("Encrypted as (%s). \n", encrypted);
    boolean success = storeMessage();
    if (success) {
      confirmedAnimation(3);
    } else {
      refusedAnimation(3);
    }
    Serial.printf("Message written\n");
  }
  else if (type == 1 ) {
    Serial.print("Acceleration: ");
    Serial.println(acceleration);
  }
}

void receiveBinaryPuzzleLooping() {
  int type = detectMovement();
  int leftRight = getXSign();
  if (type < 1) {
    displayHighlighted();
  }
  else if (type == 1) {
    switchPuzzleLed(highlighted);
  }
  else if (leftRight == -1) {
    highlight(1);
  }
  else if (leftRight == 1) {
    highlight(-1);
  }
  else if (type == 2 || type == 20) {
    submitSolution();
  }
  else if (type == 10) {
    binaryFeedbackAnimation(type);
  }
  strip.show();
}

void accelerometerLoop() {
  if (! imu.accelAvailable() ) {
    imu.begin();
  }

  imu.readGyro();
  imu.readAccel();
  imu.readMag();
}

void resetSavedMovement() {
  lastMoved = 0;
  lastRotated = 0;
  movement = false;
  movementStart = 0;
  lastMovement = 0;
  lastMinimalMovement = 0;
  for (int n = 0; n < 3; n++) {
    accumForces[n] = 0;
    accumRotations[n]= 0;
    rotationExtremes[n][0] = 0;
    rotationExtremes[n][1] = 0;
    downVector[n] = 0;
  }
  accumForce = 0;
  accumRotation = 0;
  movementDuration = 0;
  measurements = 0;
  finishedMovement = false;
  type = 0;
  
  savedForce = 0;
  savedDuration = 0;
  savedMeasurements = 0;
}

