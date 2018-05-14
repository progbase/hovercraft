
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

int airDriver_FillStage = 80;
int airDriver_UnfillStage = 0;

int PIN_airDriver_1 = 14;
int PIN_airDriver_2 = 12;
Servo airDriver_1;
Servo airDriver_2;
int DEGREE_airDriver = airDriver_UnfillStage; // zero for full disable

// DiRECTION (LEFT|RIGHT) PIN 4

int DIRECTION_NORMAL = 90;
int DIRECTION_LEFT = 135;
int DIRECTION_RIGHT = 45;

int PIN_directionServo = 4;
Servo directionServo;
int DEGREE_directionServo = DIRECTION_NORMAL; // 90 is normal state

// SPEED CONTROLLER PIN 5

int speedController_stopStage = 60;
int speedController_maxSpeedStage = 180;
int speedController_step = 40;

int PIN_speedController = 5;
Servo speedController;
int DEGREE_speedController = speedController_stopStage; // 67 is minimal state of move

// @NOBADSTAGE_INIT

// signal if all okay
int noBadStage_OKAY = 0;
// signal to abort
int noBadStage_ABORT = 42;

// handler of bad events
int noBadStage_Handler = noBadStage_OKAY;

// present stage of direction
int noBadStage_DIRECTION_NOW = DIRECTION_NORMAL;
// if in normal direction
int noBadStage_DIRECTION_NORMAL = 1;

// present stage of speed controller
int noBadStage_SPEED_NOW = speedController_stopStage;

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
    if (!controller) {
      return;
    }

    // init for noBadStage
    noBadStage_Handler = noBadStage_OKAY;

    // take command from controller
    commandFromController = checkControllerAction();

    // If the incoming data is "turnright", run the directionServo_Right function
    if (commandFromController == "turnright")
    noBadStage_Handler =  directionServo_Right();

    // If the incoming data is "turnleft", run the directionServo_Left function
    else if (commandFromController == "turnleft")
    noBadStage_Handler = directionServo_Left();

    // If the incoming data is "setdefault", run the directionServo_Default function
    else if (commandFromController == "setdefault")
    noBadStage_Handler = directionServo_Default();

    // If the incoming data is "speedup", run the speedController_Up function
    else if (commandFromController == "speedup")
    noBadStage_Handler = speedController_Up();

    // If the incoming data is "speeddown", run the speedController_Down function
    else if (commandFromController == "speeddown")
    noBadStage_Handler = speedController_Down();

    // If the incoming data is "stop", run the speedController_Stop function
    else if (commandFromController == "stop")
    noBadStage_Handler = speedController_Stop();

    // If the incoming data is "maxspeed", run the speedController_Max function
    else if (commandFromController == "maxspeed")
    noBadStage_Handler = speedController_Max();

    // If the incoming data is "off", run the airDriver_Off function
    else if (commandFromController == "off") {
      noBadStage_Handler = airDriver_Off();
    }

    // If the incoming data is "on", run the airDriver_On function
    else if (commandFromController == "on")
    noBadStage_Handler = airDriver_On();

    // events
    airDriver_1.write(DEGREE_airDriver);
    airDriver_2.write(DEGREE_airDriver);

    directionServo.write(DEGREE_directionServo);

    speedController.write(DEGREE_speedController);

    // @NOBADSTAGE_HANDLER
    if (noBadStage_Handler != noBadStage_OKAY) {

      // if (noBadStage_Handler == noBadStage_ABORT) {
        airDriver_1.write(DEGREE_airDriver);
        airDriver_2.write(DEGREE_airDriver);
        speedController_Stop();
        speedController.write(DEGREE_speedController);
        directionServo_Default();
        directionServo.write(DEGREE_directionServo);
      // }

/*
      else if (noBadStage_Handler == noBadStage_SIGNAL_DIRECTION) {
        speedController_Stop();
        speedController.write(DEGREE_speedController);
        directionServo_Default();
        directionServo.write(DEGREE_directionServo);
      }

      else if (noBadStage_Handler == noBadStage_SIGNAL_SPEED) {
        speedController_Stop();
        speedController.write(DEGREE_speedController);
      }
*/

    }


// ++++++++++++++++++++++++++++++++++++++++++++++++ Run function according to incoming signal from app ++++++++++++++++++++++++++++++++++++++++++++++++++
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

int airDriver_On (void) {

  DEGREE_airDriver = airDriver_FillStage;

  return noBadStage_OKAY;
}

int airDriver_Off (void) {

    DEGREE_airDriver = airDriver_UnfillStage;

    if (noBadStage_SPEED_NOW != speedController_stopStage)
      return noBadStage_ABORT;
    if (noBadStage_DIRECTION_NORMAL != 1)
      return noBadStage_ABORT;

    return noBadStage_OKAY;
}

// @EVENTS_DIRECTIONCONTROLLER

int directionServo_Right (void) {

  if (noBadStage_DIRECTION_NOW == DIRECTION_RIGHT)
    return noBadStage_OKAY;

  DEGREE_directionServo = DIRECTION_RIGHT;

  noBadStage_DIRECTION_NOW = DIRECTION_RIGHT;
  noBadStage_DIRECTION_NORMAL = 0;

  return noBadStage_OKAY;
}

int directionServo_Left (void) {

  if (noBadStage_DIRECTION_NOW == DIRECTION_LEFT)
    return noBadStage_OKAY;

  DEGREE_directionServo = DIRECTION_LEFT;

  noBadStage_DIRECTION_NOW = DIRECTION_LEFT;
  noBadStage_DIRECTION_NORMAL = 0;

  return noBadStage_OKAY;
}

int directionServo_Default (void) {

  DEGREE_directionServo = DIRECTION_NORMAL;

  noBadStage_DIRECTION_NORMAL = 1;
  noBadStage_DIRECTION_NOW = DIRECTION_NORMAL;

  return noBadStage_OKAY;
}

// @EVENTS_SPEEDCONTROLLER

int speedController_Up (void) {

  if (noBadStage_SPEED_NOW == speedController_maxSpeedStage)
    return noBadStage_OKAY;

  else if (noBadStage_SPEED_NOW > speedController_maxSpeedStage || DEGREE_speedController > speedController_maxSpeedStage) {

       DEGREE_speedController += speedController_maxSpeedStage;
       noBadStage_SPEED_NOW = DEGREE_speedController;

       return noBadStage_OKAY;
  }

  DEGREE_speedController += speedController_step;

  noBadStage_SPEED_NOW = DEGREE_speedController;

  return noBadStage_OKAY;
}

int speedController_Down (void) {

  if (noBadStage_SPEED_NOW == speedController_stopStage)
    return noBadStage_OKAY;

  else if (noBadStage_SPEED_NOW < speedController_stopStage || DEGREE_speedController < speedController_stopStage) {

       DEGREE_speedController -= speedController_stopStage;
       noBadStage_SPEED_NOW = DEGREE_speedController;

       return noBadStage_OKAY;
  }

  DEGREE_speedController -= speedController_step;

  noBadStage_SPEED_NOW = DEGREE_speedController;

  return noBadStage_OKAY;
}

int speedController_Max (void) {

  if (noBadStage_SPEED_NOW == speedController_maxSpeedStage)
    return noBadStage_OKAY;

  DEGREE_speedController = speedController_maxSpeedStage;

  noBadStage_SPEED_NOW = DEGREE_speedController;

  return noBadStage_OKAY;
}

int speedController_Stop (void) {

   DEGREE_speedController = speedController_stopStage;

   noBadStage_SPEED_NOW = DEGREE_speedController;

   return noBadStage_OKAY;
 }
