/* Human_Interface_Controller

    by Corentin Bazille
    Upload this code to an Arduino UNO following the pin connection diagram.

    File created 07th May, 2018.

    Last modification 11th May, 2018
    by Corentin Bazille
*/

/************************************************************************************/

/* Compilation stats
 *  Compile with DEBUG defined:     5620 bytes, 17% on UNO
 *  Compile without DEBUG defined:  4554 bytes, 14% on UNO
 */

/* arduino libraries */
#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

/* AVR libraries */
#include <avr/interrupt.h>

/* appendix code files */
#include "Configuration.h"

/* global variables and flags */
volatile byte starterFlag, hasStartedFlag, hasEndedFlag, colorFlag, color = 0;
volatile byte counter = COUNTER_INIT_VALUE;

/* special characters for LCD screen */
byte cube1[8] = {0x0, 0x0, 0x3, 0x4, 0x8, 0x10, 0x1F, 0x10};
byte cube2[8] = {0x0, 0x0, 0x1F, 0x0, 0x1, 0x2, 0x1C, 0x4};
byte cube3[8] = {0x0, 0x0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
byte cube4[8] = {0x10, 0x0, 0x10, 0x10, 0x10, 0x1F, 0x0, 0x0};
byte cube5[8] = {0x4, 0x4, 0x5, 0x6, 0x6, 0x1C, 0x0, 0x0};
byte cube6[8] = {0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
byte batteryMid[8] = {0x0, 0x0, 0x1E, 0x1B, 0x1B, 0x1E, 0x0, 0x0};
byte clk[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
byte og[8] = {0x8, 0x14, 0x14, 0x8, 0x3, 0x4, 0x5, 0x3};

/* library constructors */
LiquidCrystal_I2C lcd(0x27, 16, 2); // lcd display at i2c adress 0x27, 16x2

/*
   Setup
*/

void setup() {
    // pin definitions
  pinMode(STARTER, INPUT);
  attachInterrupt(digitalPinToInterrupt(STARTER), starterISR, RISING);
  pinMode(COLOR, INPUT);
  attachInterrupt(digitalPinToInterrupt(COLOR), colorISR, RISING);
  pinMode(BASE_BOARD_START, OUTPUT);
  digitalWrite(BASE_BOARD_START, LOW);

  // reading of the value of the color module signal to initialise
  color = digitalRead(COLOR);

  // LCD display initialisation
  lcd.init();
  lcd.backlight();

#ifndef DEBUG
  // Splash screen display
  lcd.createChar(2, cube1);
  lcd.createChar(3, cube2);
  lcd.createChar(4, cube3);
  lcd.createChar(5, cube4);
  lcd.createChar(6, cube5);
  lcd.createChar(7, cube6);
  printLCD(0, 0, "Kubz");
  printLCD(13, 1, "V.3");
  printLCD(0, 1, "Valrob");
  lcd.setCursor(7, 0);
  lcd.write(byte(2)); lcd.write(byte(3)); lcd.write(byte(4));
  lcd.setCursor(7, 1);
  lcd.write(byte(5)); lcd.write(byte(6)); lcd.write(byte(7));
  delay(SPLASH_SCREEN_TIMEOUT);
  lcd.clear();

  lcd.createChar(0, batteryMid);
  lcd.createChar(1, clk);
  lcd.createChar(2, og);
  // timer
  lcd.setCursor(12, 0);
  lcd.write(byte(1));
  updateCountdown();
  // color
  lcd.setCursor(0, 0);
  lcd.write(byte(2));
  updateColor();
  // status label
  printLCD(0, 1, "Status:");
  updateStatus("Waiting");
#endif

#ifdef DEBUG
  Serial.begin(9600);
  Serial.println("***DEBUG MODE***");
  Serial.println("***DO NOT USE AS MATCH PROGRAM***");
  Serial.println("Check connections:");
  Serial.print("Starter module: "); Serial.println(STARTER);
  Serial.print("Color module: "); Serial.println(COLOR);
  Serial.print("Base board start interrupt: "); Serial.println(BASE_BOARD_START);
  printLCD(0, 0, "! DEBUG !");
  Serial.print("Counter value: "); Serial.println(counter);
  Serial.print("Color: "); Serial.println((color ? "Orange" : "Green"));
#endif

}

/*
   Starter module ISR
   Sets starterFlag to 1 after disconnecting jack plug from module
   Delay introduced to prevent any rebound
*/
void starterISR() {
  delayMicroseconds(15000); // = 15ms
  starterFlag = 1;
}

/*
   Color module ISR
   Uses pin change interrupts to work.
   Sets colorFlag to 1 after debouncing.
   Color flag is used in main loop to change the value of the color variable
   and update that part of the LCD only if necessary.
*/
 void colorISR() {
  delayMicroseconds(15000); // = 15ms
  colorFlag = 1;
}

/*
   Print LCD
   Prints string at location specified using LiquidCrystal_I2C library
   (solves problems encountered during testing)
*/
void printLCD(int col, int lin, char str[]) {
  lcd.setCursor(col, lin);
  for (int i = 0; str[i] != '\0'; i++) {
    lcd.write(str[i]);
  }
}

/*
   Send Interrupt to Slave on Pin
   Sends interrupt signal to slave on pin to signify end of one specific action:
   lifting of cage, start of program, end of match
*/
void sendInterruptToSlave(byte pin) {
  digitalWrite(pin, HIGH);
  delay(INTERRUPT_TO_SLAVE_DURATION);
  digitalWrite(pin, LOW);
}

/*
   Decrease Counter ISR
   ISR for Timer 1, decreases seconds counter until end of match every second
*/
void decreaseCounter() {
  counter -= 1;
}

/*
   Update Cntdw
   Updates countdown to end of match
*/
void updateCountdown() {
  if (hasEndedFlag == 0) { //inhibits countdown if match has ended
    char count[3];
    if ((0 < counter) && (counter <= COUNTER_INIT_VALUE)) {
      count[0] = counter / 100;
      count[1] = (counter - 100 * count[0]) / 10;
      count[2] = (counter - 100 * count[0] - 10 * count[1]);
      count[0] = count[0] + '0'; count[1] = count[1] + '0'; count[2] = count[2] + '0';
      printLCD(13, 0, count);
    }
    else if (counter == 0) {
      hasEndedFlag = 1;
      Timer1.detachInterrupt();
    }
  }
  else printLCD(13, 0, "End");
}

/*
   Update Color
   Updates color name on LCD screen
*/
void updateColor() {
  if (color == 1) {
    printLCD(1, 0, "Orange");
#ifdef DEBUG
    Serial.println("Color: orange");
#endif
  }
  else {
    printLCD(1, 0, "Green ");
#ifdef DEBUG
    Serial.println("Color: green");
#endif
  }
}

/*
   Update Status
   Updates status of robot on LCD screen.
   Send any string (not too long), will be justified to right of screen
*/
void updateStatus(char stat[]) {
  byte i;
  for (i = 0; stat[i] != '\0'; i++);
  printLCD(16 - i, 1, stat);
}

/*
   Loop
*/

void loop() {
#ifdef DEBUG
  printLCD(0, 0, "! DEBUG !"); // display warning at all times and over all text
#endif
  if ((starterFlag == 1) && (hasStartedFlag == 0)) { // if starterFlag == 1 and hasStartedFlag == 0
    sendInterruptToSlave(BASE_BOARD_START); // send interrupt signal to base board to signify start of match
    starterFlag = 0; // invert flag once dealt with
    hasStartedFlag = 1; // set the flag telling this board that the match has started
    updateStatus("Running");
    // timer initialisation
    Timer1.initialize(1000000); // init at 1s
    Timer1.attachInterrupt(decreaseCounter);
  }
  if (hasEndedFlag == 1) {
    updateStatus("Score prev.: 145"); // set goal score here
#ifdef DEBUG
    Serial.println("Countdown ended");
#endif
  }
  if (colorFlag == 1) {
    color = !color;
    updateColor();
    colorFlag = 0;
  }
  updateCountdown();
}
