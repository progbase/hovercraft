
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

// @LOOP

void loop () {

    // If the server available, run the "checkClient" function

    controller = server.available();

    if (!controller) return;

    commandFromController = checkControllerAction();

    // If the incoming data is "turnright", run the directionServo_Right function
    if (commandFromController == "turnright") directionServo_Right();

    // If the incoming data is "turnleft", run the directionServo_Left function
    else if (commandFromController == "turnleft") directionServo_Left();

    // If the incoming data is "setdefault", run the directionServo_Default function
    else if (commandFromController == "setdefault") directionServo_Default();

    // If the incoming data is "speedup", run the speedController_Up function
    else if (commandFromController == "speedup") speedController_Up();

    // If the incoming data is "speeddown", run the speedController_Down function
    else if (commandFromController == "speeddown") speedController_Down();

    // If the incoming data is "stop", run the speedController_Stop function
    else if (commandFromController == "stop") speedController_Stop();

    // If the incoming data is "maxspeed", run the speedController_Max function
    else if (commandFromController == "maxspeed") speedController_Max();

    // If the incoming data is "off", run the airDriver_Off function
    else if (commandFromController == "off") airDriver_Off();

    // If the incoming data is "on", run the airDriver_On function
    else if (commandFromController == "on") airDriver_On();

// ++++++++++++++++++++++++++++++++++++++++++++++++ Run function according to incoming signal from app ++++++++++++++++++++++++++++++++++++++++++++++++++
}

// @CHECK_CONTORLLER

String checkControllerAction (void) {
  while(!client.available()) delay(1);
  String request = client.readStringUntil('\r');
  Serial.println(request);
  request.remove(0, 5);
  request.remove(request.length()-9,9);
  return request;
}

// @EVENTS_AIRDRIVER
