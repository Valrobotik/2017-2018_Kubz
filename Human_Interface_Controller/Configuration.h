#ifndef CONFIG_H_
#define CONFIG_H_

// debug mode
//#define DEBUG // comment out if not in debug mode

// pins
#define STARTER 2 // pin for starter module signal
#define COLOR 3 // pin for color module signal: 1 for RED/ORANGE, 0 for GREEN
#define BASE_BOARD_START 4 // pin connected to interrupt pin for start of base board

// parameters
#define INTERRUPT_TO_SLAVE_DURATION 1 // duration of interrupt to send to slave
#define BB_ISR_DELAY 15 // duration of delay for BB ISR, avoid repetition of one same interrupt
#define COUNTER_INIT_VALUE 100 // initial value for timer countdown
#define SPLASH_SCREEN_TIMEOUT 2000 // splash screen timeout in ms
#define TOWER_HEIGHT 5 // allows to choose how many times the stacking loop repeats itself
#define MOVEMENT_HEIGHT 70 // height of cage movement, in mm

#endif // ifndef CONFIG_H_
