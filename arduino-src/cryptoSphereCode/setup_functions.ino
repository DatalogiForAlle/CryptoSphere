
void baseSetup() {
  Serial.begin(9600);

  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;

  if (!imu.begin()) {
    Serial.println("Can't connect to 9DoF");
    while (1) {
      ;
    }
  }

  randomSeed(analogRead(0));

  pinMode(LED_BUILTIN, OUTPUT);
  strip.begin();
  strip.setBrightness(brightness);
  strip.show();
  
  connectToWiFi(networkName, networkPswd);
  registerDevice();
  websocket.begin();
  websocket.onEvent(handleWSEvent);
  confirmedAnimation(5);
  
}

void connectToWiFi(const char * ssid, const char * pwd)
{
  int led = 0;
  Serial.println("Connecting to WiFi network: " + String(ssid));
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) 
  {
    led = (led + 1) % NUM_LEDS; // Flip ledState
    delay(200);
    Serial.print(".");
    updateAllLeds(strip.Color(0,0,0), false);
    updateLed(led, strip.Color(0,150,150));
  }
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  digitalWrite(LED_BUILTIN, LOW);
  loopState = WAITING;
}

void registerDevice()
{
  if (!connectToServer()) {
    return;
  }
  
  Serial.println("Registrering device...");

  DynamicJsonBuffer jsonBuffer(2048);
  JsonObject& root = jsonBuffer.createObject();
  char * url = "/cryptosphere/registerDevice.php";
  root["userID"] = userID;
  root["localIP"] = ipToString(WiFi.localIP());

  char content[1024] = { 0 };
  root.printTo((char*)content, root.measureLength() + 1);
  request(String(url), String(content), "POST", false);
  Serial.println("Done");
}

void resetAll() {
  resetSavedMovement();
  resetLeds();
  loadedMessage[0] = '\0';
  encrypted[0] = '\0';
  recipient[0] = '\0';
  msgLength = 0;
  encryptionLevel = 0;
  loopState = WAITING;
  highlighted = 0;
  highlightedColor = 0;
  lastLevel = -1;
  lastRotToColor = 0;
  nextThres = -1;
  lastThres = -1;
}
