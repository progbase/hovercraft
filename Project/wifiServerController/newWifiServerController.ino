#include <Servo.h>
#include <ESP8266WiFi.h>

#define TURN_RIGHT 0
#define TURN_LEFT 1
#define TURN_DEFAULT 2
#define SPEED_UP 3
#define SPEED_DOWN 4
#define SPEED_STOP 5
#define SPEED_MAX 6
#define STATE_ON 7
#define STATE_OFF 8
#define COMMAND_NONE 9
#define AIRDRIVER_STAGE_UNFILL 100
// #define AIRDRIVER_STAGE_LOW 105
// #define AIRDRIVER_STAGE_MIDDLE 110
// #define AIRDRIVER_STAGE_MIDDLEHIGH 111
// #define AIRDRIVER_STAGE_HIGH 113
#define AIRDRIVER_STAGE_FILL 115
#define PIN_AIRDRIVER_FIRST 14
#define PIN_AIRDRIVER_SECOND 12
#define PIN_DIRECTION 4
#define PIN_SPEEDCONTROLLER 5
#define SPEEDCONTROLLER_SPEED_STOP 30
#define SPEEDCONTROLLER_SPEED_FIRST 40
#define SPEEDCONTROLLER_SPEED_SECOND 120
#define SPEEDCONTROLLER_SPEED_THIRD 160
// #define SPEEDCONTROLLER_SPEED_MAX  180
#define DIRECTION_NORMAL 98 // == 90*
#define DIRECTION_LEFT 58 // == 120*
#define DIRECTION_RIGHT 138 // == 60*
const char * ssid = "ShawarmaHost";
const char * password = "14881488";

WiFiClient controller;
WiFiServer server(80); //port
String  commandFromController ="";
Servo firstAirDriver;
Servo secondAirDriver;
Servo speedController;
Servo directionServo;
byte firstAirDriverCurrentDegree = AIRDRIVER_STAGE_UNFILL;
byte secondAirDriverCurrentDegree = AIRDRIVER_STAGE_UNFILL;
byte directionDegree = DIRECTION_NORMAL;
byte speedControllerCurrentDegree = SPEEDCONTROLLER_SPEED_STOP;

class Command {
public:
  virtual void execute() = 0;
  virtual ~Command() {}
};

class Invoker {
public:
  virtual void execute(byte code) = 0;
  virtual ~Invoker() {}
};

class AirDriver_On : public Command {
public:
  AirDriver_On();
  virtual void execute();
};
AirDriver_On::AirDriver_On() {}
void AirDriver_On::execute() {
  firstAirDriverCurrentDegree = AIRDRIVER_STAGE_FILL;
  secondAirDriverCurrentDegree = AIRDRIVER_STAGE_FILL;
  firstAirDriver.write(firstAirDriverCurrentDegree);
  secondAirDriver.write(secondAirDriverCurrentDegree);
}

class AirDriver_Off : public Command {
public:
  AirDriver_Off();
  virtual void execute();
};
AirDriver_Off::AirDriver_Off() {}
void AirDriver_Off::execute() {
  firstAirDriverCurrentDegree = AIRDRIVER_STAGE_UNFILL;
  secondAirDriverCurrentDegree = AIRDRIVER_STAGE_UNFILL;
  firstAirDriver.write(firstAirDriverCurrentDegree);
  secondAirDriver.write(secondAirDriverCurrentDegree);
  if (speedControllerCurrentDegree != SPEEDCONTROLLER_SPEED_STOP) {
    speedControllerCurrentDegree= SPEEDCONTROLLER_SPEED_STOP;
    speedController.write(speedControllerCurrentDegree);
  }
  if (directionDegree != DIRECTION_NORMAL) {
    directionDegree = DIRECTION_NORMAL;
    directionServo.write(directionDegree);
  }
}

class Direction_Right : public Command {
public:
  Direction_Right();
  virtual void execute();
};
Direction_Right::Direction_Right() {}
void Direction_Right::execute() {
  if (directionDegree == DIRECTION_RIGHT)
    return;
  directionDegree = DIRECTION_RIGHT;
}

class Direction_Left : public Command {
public:
  Direction_Left();
  virtual void execute();
};
Direction_Left::Direction_Left() {}
void Direction_Left::execute() {
  if (directionDegree == DIRECTION_LEFT)
    return;
  directionDegree = DIRECTION_LEFT;
}

class Direction_Default : public Command {
public:
  Direction_Default();
  virtual void execute();
};
Direction_Default::Direction_Default() {}
void Direction_Default::execute() {
  directionDegree = DIRECTION_NORMAL;
}

class SpeedController_Up : public Command {
public:
  SpeedController_Up();
  virtual void execute();
};
SpeedController_Up::SpeedController_Up() {}
void SpeedController_Up::execute()  {
  if (speedControllerCurrentDegree >= SPEEDCONTROLLER_SPEED_THIRD)
    return;
  else if (speedControllerCurrentDegree == SPEEDCONTROLLER_SPEED_STOP) {
    for (int smoothSpdStrt = SPEEDCONTROLLER_SPEED_STOP; smoothSpdStrt < SPEEDCONTROLLER_SPEED_FIRST; smoothSpdStrt++) {
      speedControllerCurrentDegree += 1;
      speedController.write(speedControllerCurrentDegree);
      delay(100);
    }
    speedControllerCurrentDegree = SPEEDCONTROLLER_SPEED_FIRST;
  }
  else if (speedControllerCurrentDegree == SPEEDCONTROLLER_SPEED_FIRST)
    speedControllerCurrentDegree = SPEEDCONTROLLER_SPEED_SECOND;
  else if (speedControllerCurrentDegree == SPEEDCONTROLLER_SPEED_SECOND)
    speedControllerCurrentDegree = SPEEDCONTROLLER_SPEED_THIRD;
}

class SpeedController_Down : public Command {
public:
  SpeedController_Down();
  virtual void execute();
};
SpeedController_Down::SpeedController_Down() {}
void SpeedController_Down::execute() {
  if (speedControllerCurrentDegree <= SPEEDCONTROLLER_SPEED_STOP)
    return;
  else if (speedControllerCurrentDegree == SPEEDCONTROLLER_SPEED_THIRD)
    speedControllerCurrentDegree = SPEEDCONTROLLER_SPEED_SECOND;
  else if (speedControllerCurrentDegree == SPEEDCONTROLLER_SPEED_SECOND)
    speedControllerCurrentDegree = SPEEDCONTROLLER_SPEED_FIRST;
  else if (speedControllerCurrentDegree == SPEEDCONTROLLER_SPEED_FIRST)
    speedControllerCurrentDegree = SPEEDCONTROLLER_SPEED_STOP;
}

class SpeedController_Max : public Command {
public:
  SpeedController_Max();
  virtual void execute();
};
SpeedController_Max::SpeedController_Max() {}
void SpeedController_Max::execute() {
  if (speedControllerCurrentDegree >= SPEEDCONTROLLER_SPEED_THIRD)
    return;
  speedControllerCurrentDegree = SPEEDCONTROLLER_SPEED_THIRD;
}

class SpeedController_Stop : public Command {
public:
  SpeedController_Stop();
  virtual void execute();
};
SpeedController_Stop::SpeedController_Stop() {}
void SpeedController_Stop::execute() {
  speedControllerCurrentDegree = SPEEDCONTROLLER_SPEED_STOP;
}

class ConstantInvoker : public Invoker {
private:
  static const byte avaibleCommandsCount= COMMAND_NONE;
  Command** avaibleCommands= new Command*[avaibleCommandsCount];
public:
  ConstantInvoker();
  virtual void execute(byte code);
};
ConstantInvoker::ConstantInvoker() {
    avaibleCommands[TURN_RIGHT]= new Direction_Right();
    avaibleCommands[TURN_LEFT]= new Direction_Left();
    avaibleCommands[TURN_DEFAULT]= new Direction_Default();
    avaibleCommands[SPEED_UP]= new SpeedController_Up();
    avaibleCommands[SPEED_DOWN]= new SpeedController_Down();
    avaibleCommands[SPEED_STOP]= new SpeedController_Stop();
    avaibleCommands[SPEED_MAX]= new SpeedController_Max();
    avaibleCommands[STATE_ON]= new AirDriver_On();
    avaibleCommands[STATE_OFF]= new AirDriver_Off();
}
void ConstantInvoker::execute(byte code) {
  if (code < avaibleCommandsCount) {
    avaibleCommands[code]->execute();
  }
}
Invoker * invoker= new ConstantInvoker();

byte checkControllerAction (void) {
  while(!controller.available()) delay(1);
  String request = controller.readStringUntil('\r');

  // delete http:/someIp/ from http:/someIp/command
  request.remove(0, 5);
  request.remove(request.length()-9,9);

  if (request == "turnright") {
    return TURN_RIGHT;
  }
  else if (request == "turnleft") {
    return TURN_LEFT;
  }
  else if (request == "setdefault") {
    return TURN_DEFAULT;
  }
  else if (request == "speedup") {
    return SPEED_UP;
  }
  else if (request == "speeddown") {
    return SPEED_DOWN;
  }
  else if (request == "stop") {
    return SPEED_STOP;
  }
  else if (request == "maxspeed") {
    return SPEED_MAX;
  }
  else if (request == "off") {
    return STATE_OFF;
  }
  else if (request == "on") {
    return STATE_ON;
  } else return COMMAND_NONE;
}

void setup () {
  firstAirDriver.attach(PIN_AIRDRIVER_FIRST);
  secondAirDriver.attach(PIN_AIRDRIVER_SECOND);
  directionServo.attach(PIN_DIRECTION);
  directionServo.write(DIRECTION_NORMAL);
  speedController.attach(PIN_SPEEDCONTROLLER);
  server.begin();
}

void loop () {
  controller = server.available(); if (!controller) return;
  invoker->execute(checkControllerAction());

  firstAirDriver.write(firstAirDriverCurrentDegree);
  secondAirDriver.write(secondAirDriverCurrentDegree);
  directionServo.write(directionDegree);
  speedController.write(speedControllerCurrentDegree);
}
