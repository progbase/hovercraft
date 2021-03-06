
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

// controller degree for fill with air
int airDriver_Stage_Fill = 115;
// controller degree for fill with air
int airDriver_Stage_Unfill = 100;

int airDriver_Stage_MiddleFill = 110;
int airDriver_Stage_LowFill = 105;
int airDriver_Stage_HighFill = 113;
int airDriver_Stage_MiddleHighFill = 111;

int smoothIndex = airDriver_Stage_Unfill;

int PIN_airDriver_1 = 14;
int PIN_airDriver_2 = 12;
Servo airDriver_1;
Servo airDriver_2;

// controller degree to unfill stage
int DEGREE_airDriver_2 = airDriver_Stage_Unfill;
int DEGREE_airDriver_1 = airDriver_Stage_Unfill;

// DiRECTION (LEFT|RIGHT) PIN 4

// servo motor is not really correct in degrees
int DIRECTION_NORMAL = 98; // == 90*
int DIRECTION_LEFT = 58; // == 120*
int DIRECTION_RIGHT = 138; // == 60*

int PIN_directionServo = 4;
Servo directionServo;
int DEGREE_directionServo = DIRECTION_NORMAL; // degrees to normal stage

// SPEED CONTROLLER PIN 5

int speedController_stopStage = 30;
int speedController_FirstSpeed = 40;
int speedController_SecondSpeed = 120;
int speedController_ThirdSpeed = 160;
// int speedController_maxSpeedStage = 180;
// int speedController_step = 50;

int PIN_speedController = 5;
Servo speedController;
int DEGREE_speedController = speedController_stopStage; // to minimal state of move

// @SETUP

void setup () {

  // servo init

  airDriver_1.attach(PIN_airDriver_1);
  airDriver_2.attach(PIN_airDriver_2);

  directionServo.attach(PIN_directionServo);

  directionServo.write(DIRECTION_NORMAL);

  speedController.attach(PIN_speedController);

  // start serial for debugging

//  Serial.begin(115200);
//  Serial.println("Setup ");


  // wifi server inits
  server.begin();

}

// @LOOP

void loop () {

    // if the server available
    controller = server.available();
    if (!controller) return;

    // take command from controller
    commandFromController = checkControllerAction();

    // If the incoming command is "turnright", run the directionServo_Right function
    if (commandFromController == "turnright")
      directionServo_Right();

    // If the incoming command is "turnleft", run the directionServo_Left function
    else if (commandFromController == "turnleft")
      directionServo_Left();

    // If the incoming command is "setdefault", run the directionServo_Default function
    else if (commandFromController == "setdefault")
      directionServo_Default();

    // If the incoming command is "speedup", run the speedController_Up function
    else if (commandFromController == "speedup")
      speedController_Up();

    // If the incoming command is "speeddown", run the speedController_Down function
    else if (commandFromController == "speeddown")
      speedController_Down();

    // If the incoming command is "stop", run the speedController_stopStage function
    else if (commandFromController == "stop")
      speedController_Stop();

    // If the incoming command is "maxspeed", run the speedController_Max function
    else if (commandFromController == "maxspeed")
      speedController_Max();

    // If the incoming command is "off", run the airDriver_Off function
    else if (commandFromController == "off")
      airDriver_Off();

    // If the incoming command is "on", run the airDriver_On function
    else if (commandFromController == "on")
      airDriver_On();

    // events write
    airDriver_1.write(DEGREE_airDriver_1);
    airDriver_2.write(DEGREE_airDriver_2);

    directionServo.write(DEGREE_directionServo);

    speedController.write(DEGREE_speedController);
}

// @CHECK_CONTORLLER

String checkControllerAction (void) {

  // wait for client signal
  while(!controller.available()) delay(1);

  // take command
  String request = controller.readStringUntil('\r');

//  Serial.println(request);

  // delete http:/someIp/ from http:/someIp/command
  request.remove(0, 5);

  request.remove(request.length()-9,9);

  // return it
  return request;
}

// @EVENTS_AIRDRIVER

// starts air in
void airDriver_On (void) {

  // smooth airdriver filling
/*
  for (smoothIndex = airDriver_Stage_Unfill; smoothIndex <= airDriver_Stage_Fill; smoothIndex++) {

    DEGREE_airDriver_1 = smoothIndex;
    DEGREE_airDriver_2 = smoothIndex;

    airDriver_1.write(DEGREE_airDriver_1);
    airDriver_2.write(DEGREE_airDriver_2);

    delay(200);
  }
*/

  // thunder airdriver filling

    DEGREE_airDriver_1 = airDriver_Stage_Fill;
    DEGREE_airDriver_2 = airDriver_Stage_Fill;

    airDriver_1.write(DEGREE_airDriver_1);
    airDriver_2.write(DEGREE_airDriver_2);

}

// off the air in (and probably all)
void airDriver_Off (void) {

  // smooth airdriver stop
/*
    for (smoothIndex = DEGREE_airDriver_1; smoothIndex >= airDriver_Stage_Unfill; smoothIndex--) {

      DEGREE_airDriver_1 = smoothIndex;
      DEGREE_airDriver_2 = smoothIndex;

      airDriver_1.write(DEGREE_airDriver_1);
      airDriver_2.write(DEGREE_airDriver_2);

      delay(200);
    }

*/

  // thinder airdriver stop

    DEGREE_airDriver_1 = airDriver_Stage_Unfill;
    DEGREE_airDriver_2 = airDriver_Stage_Unfill;

    airDriver_1.write(DEGREE_airDriver_1);
    airDriver_2.write(DEGREE_airDriver_2);

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

// change direction of servo to right
void directionServo_Right (void) {

  if (DEGREE_directionServo == DIRECTION_RIGHT)
    return;

  DEGREE_directionServo = DIRECTION_RIGHT;

}

// change direction of servo to left
void directionServo_Left (void) {

  if (DEGREE_directionServo == DIRECTION_LEFT)
    return;

  DEGREE_directionServo = DIRECTION_LEFT;

}

// change direction of servo to default
void directionServo_Default (void) {

  DEGREE_directionServo = DIRECTION_NORMAL;

}

// @EVENTS_SPEEDCONTROLLER

// speedup the speed by step
void speedController_Up (void) {

  if (DEGREE_speedController >= speedController_ThirdSpeed)
    return;
  else if (DEGREE_speedController == speedController_stopStage) {
    for (int smoothSpdStrt = speedController_stopStage; smoothSpdStrt < speedController_FirstSpeed; smoothSpdStrt++) {
      DEGREE_speedController += 1;
      speedController.write(DEGREE_speedController);
      delay(100);
    }
    DEGREE_speedController = speedController_FirstSpeed;
  }
  else if (DEGREE_speedController == speedController_FirstSpeed)
    DEGREE_speedController = speedController_SecondSpeed;
  else if (DEGREE_speedController == speedController_SecondSpeed)
    DEGREE_speedController = speedController_ThirdSpeed;

}

// speeddown the speed by step
void speedController_Down (void) {

  if (DEGREE_speedController <= speedController_stopStage)
    return;
  else if (DEGREE_speedController == speedController_ThirdSpeed)
    DEGREE_speedController = speedController_SecondSpeed;
  else if (DEGREE_speedController == speedController_SecondSpeed)
    DEGREE_speedController = speedController_FirstSpeed;
  else if (DEGREE_speedController == speedController_FirstSpeed)
    DEGREE_speedController = speedController_stopStage;

}

// speedup the speed to max speed position
void speedController_Max (void) {

 if (DEGREE_speedController == speedController_stopStage)
   speedController_Up();

  if (DEGREE_speedController >= speedController_ThirdSpeed)
    return;

  DEGREE_speedController = speedController_ThirdSpeed;

}

// speeddown the speed to stop position
void speedController_Stop (void) {

   DEGREE_speedController = speedController_stopStage;

 }
