
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

// MODULES

// AIR PIN 2

int airDriver_FillStage = 140;
int airDriver_UnfillStage = 0;

int PIN_airDriver = 2;
Servo airDriver;
int DEGREE_airDriver = airDriver_UnfillStage; // zero for full disable

// DiRECTION (LEFT|RIGHT) PIN 4

int DIRECTION_NORMAL = 90;
int DIRECTION_LEFT = 135;
int DIRECTION_RIGHT = 45;

int PIN_directionServo = 4;
Servo directionServo;
int DEGREE_directionServo = DIRECTION_NORMAL; // 90 is normal state

// SPEED CONTROLLER PIN 5

int speedController_stopStage = 65;

int PIN_speedController = 5;
Servo speedController;
int DEGREE_speedController = speedController_stopStage; // 67 is minimal state of move

// @NOBADSTAGE_INIT

// handler of bad events
int noBadStage_Handler = noBadStage_OKAY;

// signal if all okay
int noBadStage_OKAY = 0;
// signal to abort
int noBadStage_ABORT = 42;

// if overfill or try to unfill void
int noBadStage_AIR_FILLED = 0;
// error of filling
int noBadStage_SIGNAL_AIR = 9;

// present stage of direction
int noBadStage_DIRECTION_NOW = DIRECTION_NORMAL;
// if in normal direction
int noBadStage_DIRECTION_NORMAL = 1;
// if any problem detected
int noBadStage_SIGNAL_DIRECTION = 13;

// present stage of speed controller
int noBadStage_SPEED_NOW = speedController_stopStage;
// signal if anything goes bad
int noBadStage_SIGNAL_SPEED = 17;

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
  Serial.begin(115200);
  Serial.println("Setup ");
  /*
  */

}

// @LOOP

void loop () {

    // if the server available
    controller = server.available();
    if (!controller) return;

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
    else if (commandFromController == "off")
    noBadStage_Handler = airDriver_Off();

    // If the incoming data is "on", run the airDriver_On function
    else if (commandFromController == "on")
    noBadStage_Handler = airDriver_On();

    // @NOBADSTAGE_HANDLER
    if (noBadStage_Handler != noBadStage_OKAY) {

      if (noBadStage_Handler == noBadStage_SIGNAL_AIR) {
        if (noBadStage_AIR_FILLED == 1) {
          noBadStage_Handler = airDriver_Off();
          // @TODO
        }
      }

      else if (noBadStage_Handler == ) {

      }

      else if (noBadStage_Handler == noBadStage_SIGNAL_DIRECTION) {
        directionServo_Default();
      }

      else if (noBadStage_Handler == ) {

      }

    }


// ++++++++++++++++++++++++++++++++++++++++++++++++ Run function according to incoming signal from app ++++++++++++++++++++++++++++++++++++++++++++++++++
}

// @CHECK_CONTORLLER

String checkControllerAction (void) {

  while(!contoller.available()) delay(1);

  String request = contoller.readStringUntil('\r');

  Serial.println(request);

  request.remove(0, 5);

  request.remove(request.length()-9,9);

  return request;
}

// @EVENTS_AIRDRIVER

int airDriver_On (void) {

  if (noBadStage_AIR_FILLED == 1)
    return noBadStage_SIGNAL_AIR;

  DEGREE_airDriver = airDriver_FillStage;

  airDriver.write(DEGREE_airDriver);
  noBadStage_AIR_FILLED = 1;

  return noBadStage_OKAY;
}

int airDriver_Off (void) {

    if (noBadStage_SPEED_NOW != speedController_stopStage)
      speedController_Stop();
    if (noBadStage_DIRECTION_NORMAL != 1)
      directionServo_Default();

    if (noBadStage_AIR_FILLED == 0)
      return noBadStage_SIGNAL_AIR;

    DEGREE_airDriver = airDriver_UnfillStage;

    airDriver.write(DEGREE_airDriver);
    noBadStage_AIR_FILLED = 0;

    return noBadStage_OKAY;
}

// @EVENTS_DIRECTIONCONTROLLER

int directionServo_Right (void) {

  if (noBadStage_DIRECTION_NOW == DIRECTION_RIGHT)
    return;

  DEGREE_directionServo = DIRECTION_RIGHT;

  directionServo.write(DEGREE_directionServo);
  noBadStage_DIRECTION_NOW = DIRECTION_RIGHT;
  noBadStage_DIRECTION_NORMAL = 0;

  return noBadStage_OKAY;
}

int directionServo_Left (void) {

  if (noBadStage_DIRECTION_NOW == DIRECTION_LEFT)
    return;

  DEGREE_directionServo = DIRECTION_LEFT;

  directionServo.write(DEGREE_directionServo);
  noBadStage_DIRECTION_NOW = DIRECTION_LEFT;
  noBadStage_DIRECTION_NORMAL = 0;

  return noBadStage_OKAY;
}

int directionServo_Default (void) {

  DEGREE_directionServo = DIRECTION_NORMAL;

  directionServo.write(DEGREE_directionServo);
  noBadStage_DIRECTION_NORMAL = 1;
  noBadStage_DIRECTION_NOW = DIRECTION_NORMAL;

  return noBadStage_OKAY;
}

// @EVENTS_SPEEDCONTROLLER
