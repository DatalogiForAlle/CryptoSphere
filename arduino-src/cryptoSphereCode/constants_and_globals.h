#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WebSocketsServer.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <math.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>
#include "DelayDiku.h"
#include "CircleLeds.h"
//#include <Base64.h>

// CONSTANTS

#define WAITING 0

#define WAITINGFORCONFIRMATION 1

#define SENDBINARY  2
#define SENDCOLOR  3

#define RECEIVEBINARY  4
#define RECEIVECOLOR  5

#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

// GLOBAL VARIABLES

String accessToken;

WiFiClient client;
WiFiClientSecure fbClient;
WebSocketsServer websocket = WebSocketsServer(wsPort);
int wsNum = 0;

int ledStates[NUM_LEDS] = {0};
int ledColors[NUM_LEDS] = {0};
int flipStates[NUM_LEDS] = {1};
int logic[NUM_LEDS][NUM_LEDS] = {{0}};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, stripPin, NEO_GRB + NEO_KHZ800);

LSM9DS1 imu;

char loadedMessage[256]; //Message length limitation 255 characters!
char encrypted[256]; //Message length limitation 255 characters!
char recipient[256];
char recipient_name[256];
size_t msgLength = 0;

int encryptionLevel = 0;

float lastThres = 0;
float nextThres = 0;

int loopState = WAITING;

int highlighted = 0;
int highlightedColor = 0;
long unsigned int highlightTime = 0;
int lastMinimalMovement = -1;
