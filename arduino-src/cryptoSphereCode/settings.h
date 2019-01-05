// EDIT THESE

const char * networkName = "FemTech.dk2"; // WiFi name
const char * networkPswd = "0F76FMYHB0Q"; // WiFi password

const char * host = "datanauterne.dk"; // The server name -- Don't change this
int port = 80; // Port used for connecting to server -- Don't change
int wsPort = 80; // Web-socket port -- Don't change

const char * userID = "10156731684353211"; //"10211659890595626"; // You can find your ID in the bottom of the web-app after connecting with facebook

const int stripPin = 5; // Pin number for the data wire (Probably the yello one) to LED strip
const int NUM_LEDS = 8; // The number of LEDS on your strip

// SETTINGS

// LED brightness
char brightness = 128; //Goes from 0 to 255;

// Time in milliseconds before a shake is registered as a long shake.
const int longShakeDuration = 3000; 

// How much rotational Force is needed to trigger a rotation movement
const float rotationStartThreshold = 300.0; 

// How much force is needed to trigger a shaking movement
const float shakeStartThreshold = 2.0; 

// The average rotational force over the duration of the movement for it to be registered as a rotation
const float rotationAvgThreshold = 140; 

// The average shaking force over the duration of the movement for it to be registered as a rotation
const float shakeAvgThreshold = 0.40;

const float minimalMovement = 0.10;

const float minimalRotation = 12.0;

const int movementTimeout = 500; // Timeout between movements
