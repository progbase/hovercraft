
#include <Servo.h>
#include <ESP8266WiFi.h>

// @INIT

// define port
WiFiClient controller;
WiFiServer server(80);

// WIFI settings
const char * ssid = "ShawarmaHost";
const char * password = "14881488";

// command received from application
String  commandFromController ="";

// MODULES

// AIR PIN 2

int airDriver_Stage_Fill = 80;
int airDriver_Stage_Unfill = 0;

int PIN_airDriver_1 = 14;
int PIN_airDriver_2 = 12;
Servo airDriver_1;
Servo airDriver_2;
int DEGREE_airDriver = airDriver_Stage_Fill; // zero for full disable

// DiRECTION (LEFT|RIGHT) PIN 4

int DIRECTION_NORMAL = 90;
int DIRECTION_LEFT = 135;
int DIRECTION_RIGHT = 45;

int PIN_directionServo = 4;
Servo directionServo;
int DEGREE_directionServo = DIRECTION_NORMAL; // 90 is normal state

// SPEED CONTROLLER PIN 5

int speedController_stopStage = 60;
int speedController_maxSpeedStage = 90;
int speedController_step = 10;

int PIN_speedController = 5;
Servo speedController;
int DEGREE_speedController = speedController_stopStage; // 67 is minimal state of move

// @SETUP

void setup () {

  // servo init

  airDriver_1.attach(PIN_airDriver_1);
  airDriver_2.attach(PIN_airDriver_2);

  directionServo.attach(PIN_directionServo);

  speedController.attach(PIN_speedController);

  // start serial for debugging
  Serial.begin(115200);
  Serial.println("Setup ");
  /*
  */

  // wifi server inits
  server.begin();

}

// @LOOP

void loop () {

    // if the server available
    controller = server.available();
    if (!controller) return;

    // If the incoming data is "turnright", run the directionServo_Right function
    if (commandFromController == "turnright")
      directionServo_Right();

    // If the incoming data is "turnleft", run the directionServo_Left function
    else if (commandFromController == "turnleft")
      directionServo_Left();

    // If the incoming data is "setdefault", run the directionServo_Default function
    else if (commandFromController == "setdefault")
      directionServo_Default();

    // If the incoming data is "speedup", run the speedController_Up function
    else if (commandFromController == "speedup")
      speedController_Up();

    // If the incoming data is "speeddown", run the speedController_Down function
    else if (commandFromController == "speeddown")
      speedController_Down();

    // If the incoming data is "stop", run the speedController_Stop function
    else if (commandFromController == "stop")
      speedController_Stop();

    // If the incoming data is "maxspeed", run the speedController_Max function
    else if (commandFromController == "maxspeed")
      speedController_Max();

    // If the incoming data is "off", run the airDriver_Off function
    else if (commandFromController == "off")
      airDriver_Off();

    // If the incoming data is "on", run the airDriver_On function
    else if (commandFromController == "on")
      airDriver_On();

    // events
    airDriver_1.write(DEGREE_airDriver);
    airDriver_2.write(DEGREE_airDriver);

    directionServo.write(DEGREE_directionServo);

    speedController.write(DEGREE_speedController);

}

// @CHECK_CONTORLLER

String checkControllerAction (void) {

  while(!controller.available()) delay(1);

  String request = controller.readStringUntil('\r');

  Serial.println(request);

  request.remove(0, 5);

  request.remove(request.length()-9,9);

  return request;
}

// @SEND_CONTROLLER

/*
void sentSignalToController (String sig) {

  while(!controller.available()) delay(1);

  controller.write(sig);
}
*/

// @EVENTS_AIRDRIVER

void airDriver_On (void) {

  DEGREE_airDriver = airDriver_FillStage;

}

void airDriver_Off (void) {

    DEGREE_airDriver = airDriver_UnfillStage;

    if (DEGREE_speedController != speedController_stopStage) {
        speedController_Stop();
        speedController.write(DEGREE_speedController);
    }

    if (DEGREE_directionServo != DIRECTION_NORMAL) {
      directionServo_Default();
      directionServo.write(DEGREE_directionServo);
    }

}

// @EVENTS_DIRECTIONCONTROLLER

void directionServo_Right (void) {

  if (DEGREE_directionServo == DIRECTION_RIGHT)
    return;

  DEGREE_directionServo = DIRECTION_RIGHT;

}

void directionServo_Left (void) {

  if (DEGREE_directionServo == DIRECTION_LEFT)
    return;

  DEGREE_directionServo = DIRECTION_LEFT;

}

void directionServo_Default (void) {

  DEGREE_directionServo = DIRECTION_NORMAL;

}

// @EVENTS_SPEEDCONTROLLER

void speedController_Up (void) {

  DEGREE_speedController += speedController_step;

}

void speedController_Down (void) {

  DEGREE_speedController -= speedController_step;

}

void speedController_Max (void) {

  if (DEGREE_speedController >= speedController_maxSpeedStage)
    return;

  DEGREE_speedController = speedController_maxSpeedStage;
}

void speedController_Stop (void) {

   DEGREE_speedController = speedController_stopStage;

 }
