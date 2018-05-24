/* Cage_stepper_servo_automaton

    by Corentin Bazille
    Upload this code to an Arduino UNO following the pin connection diagram.

    File created 08th May, 2018.

    Last modification 11th May, 2018
    by Corentin Bazille
*/

/************************************************************************************/

/* Compilation stats
 *  Compile with DEBUG defined:     4108 bytes, 12% on UNO
 *  Compile without DEBUG defined:  2820 bytes, 08% on UNO
 */

/* arduino libraries */
#include <Servo.h>

/* appendix code files */
#include "Configuration.h"

/* global variables and flags */
volatile byte bbint, cube = 0;
volatile byte state;

/* library constructors */
Servo leftServo, rightServo; // create two servo objects to control both servos

void doStep(byte dir) {
  digitalWrite(EN, LOW); // enable stepper driver
  digitalWrite(DIR, dir); // set direction
  digitalWrite(STEP, LOW); // step pin init
  digitalWrite(STEP, HIGH); // step rising edge
  delayMicroseconds(HALFP); // delay
  digitalWrite(STEP, LOW); // step falling edge
  delayMicroseconds(HALFP); // delay
  digitalWrite(EN, HIGH); // disable stepper
}

/*
   Homing
   Homing function for cage.
   Makes the cage go down until it hits the homing switch, at which point it stops moving.
*/
void homing() {
  while (digitalRead(HOME) == HIGH) doStep(1);
  WAIT;
}

/*
   lift to move
   Lifts the cage (250 step/200 step) * (8 mm/step) = 10 mm to allow for free movement of robot
*/
void liftMove() {
  for (int i = 0; i < 250; i++) doStep(0);
  WAIT;
}

/*
   lift high
   Lifts the cage (1750/200)*8 = 70 mm to allow for stacking of cubes
*/
void liftHigh() {
  for (int i = 0; i < 1750; i++) doStep(0);
  WAIT;
}

/* lift to mid height
   Lowers the cage (215/200)*8 = 8.6 mm to mid height to allow for the cubes not to fall from too high when opening servos
   WARNING: always use only after being in liftHigh() position!
*/
void liftMid() {
  for (int i = 0; i < 215; i++) doStep(1);
  WAIT;
}

/* Open servos
    Opens both servos with the same but opposed angle
*/
void openServos() {
  leftServo.write(180 - SERVO_MIN_POSITION);
  rightServo.write(SERVO_MIN_POSITION);
  WAIT;
}

/* Close servos
    Closes both servos with same but opposed angle
*/
void closeServos() {
  leftServo.write(180 - SERVO_MAX_POSITION);
  rightServo.write(SERVO_MAX_POSITION);
  WAIT;
}

/*
   Setup
*/
void setup() {
  // pin definitions
  pinMode(EN, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);
  pinMode(HOME, INPUT_PULLUP);
  pinMode(BB_INT, INPUT_PULLUP); // port to connect BB to

  // servo motors configuration
  leftServo.attach(LEFTSERVO);
  rightServo.attach(RIGHTSERVO);

  // servo motor initial position
  leftServo.write(180 - SERVO_MIN_POSITION);
  rightServo.write(SERVO_MIN_POSITION);

  // automaton initialisation
  state = 1;

#ifdef DEBUG
  Serial.begin(9600);
  Serial.write("Restart");
#endif
}

/* Loop
   Main part of the loop is the state machine. Documentation is available in sketch folder.
*/
void loop() {
#ifdef DEBUG
  Serial.print("*** State: "); Serial.println(state);
  Serial.print("Cube = "); Serial.println(cube);
#endif

  /* State machine
     Documentation cf sketch folder
  */
  switch (state) {
    case 1: // initialisation state
      cube = 0;
      homing();
      openServos();
      liftMove();
      state = 12;
#ifdef DEBUG
      Serial.println("*** CHANGE STATE ***");
#endif
      break;

    case 12: // waiting state between 1 and 2
      if (BB_INT_READ == 0) {
        state = 2;
#ifdef DEBUG
        Serial.println("*** CHANGE STATE ***");
#endif
      }
      break;

    case 2: // state to pick up cube
      homing();
      closeServos();
      liftHigh();
      cube++;
      state = 23;
#ifdef DEBUG
      Serial.println("*** CHANGE STATE ***");
#endif
      break;

    case 23: // waiting state
      if (BB_INT_READ == 0) {
        state = 3;
#ifdef DEBUG
        Serial.println("*** CHANGE STATE ***");
#endif
      }
      break;

    case 3: // state to lower already built tower on cube on map
      liftMid();
      openServos();
      if (cube == (TOWER_HEIGHT - 1)) { // if the tower is one cube short of the wished height, go to state 4 which picks up the last one and lifts to moving height
        state = 4;
#ifdef DEBUG
        Serial.println("*** CHANGE STATE ***");
#endif
      }
      else { // else, go back to picking up a cube and moving to stacking height
        state = 2;
#ifdef DEBUG
        Serial.println("*** CHANGE STATE ***");
#endif
      }
      break;

    case 4: // pick up cube and go to moving height
      homing();
      closeServos();
      liftMove();
      state = 41;
#ifdef DEBUG
      Serial.println("*** CHANGE STATE ***");
#endif
      break;

    case 41: // waiting state
      if (BB_INT_READ == 0) {
        state = 1;
#ifdef DEBUG
        Serial.println("*** CHANGE STATE ***");
#endif
      }
      break;
  }
}
