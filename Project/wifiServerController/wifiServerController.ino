
#include <ESP8266WiFi.h>
#include <Servo.h>

// @INIT

// define port
WiFiClient contoller;
WiFiServer server(80);

// WIFI settings
const char * ssid = "ShawarmaHost";
const char * password = "14881488";

// command received from application
String  commandFromController ="";

// PINS

// AIR PIN 2
int PIN_airDriver = 2;
Servo airDriver;
int DEGREE_airDriver = 0; // zero for full disable

// DiRECTION (LEFT|RIGHT) PIN 4
int PIN_directionServo = 4;
Servo directionServo;
int DEGREE_directionServo = 90; // 90 is normal state

// SPEED CONTROLLER PIN 5
int PIN_speedController = 5;
Servo speedController;
int DEGREE_speedController = 65; // 67 is minimal state of move

// @SETUP

void setup () {

  // servo init

  airDriver.attach(PIN_airDriver);

  directionServo.attach(PIN_directionServo);

  speedController.attach(PIN_speedController);

  // servo state init

  airDriver.write(DEGREE_airDriver);

  directionServo.write(DEGREE_directionServo);

  speedController.write(DEGREE_speedController);

  // wifi server inits

  IPAddress ip = WiFi.localIP();

  server.begin();

  // start serial for debugging
  /*
  Serial.begin(115200);
  Serial.println("Setup ");
  */

}
