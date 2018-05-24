#ifndef _CONFIG_H_
#define _CONFIG_H_

// debug mode
//#define DEBUG // comment out if not in debug mode

// stepper motor pins
#define EN 7 // enable pin on stepper driver
#define STEP 6 // step pin on stepper driver
#define DIR 5 // direction pin on stepper driver

// servo motor pins
#define LEFTSERVO 10 // pin for left servo command
#define RIGHTSERVO 9 // pin for right servo command

// limit switch pins
#define HOME 2 // home switch pin

#define HALFP 800 // stepper driver half period

// base board "communication"
#define BB_INT 3 // high by default (pull up), active low

// mechanical configuration
#define SERVO_MIN_POSITION 15 // servo motor minimum angle
#define SERVO_MAX_POSITION 60 // servo motor maximum angle

// tower building configuration
#define TOWER_HEIGHT 5 // height of the tower to build (number of state repetitions)

// macros to make life easier
#define WAIT delay(100)
#define BB_INT_READ digitalRead(BB_INT)

#endif // #ifdef _CONFIG_H_
