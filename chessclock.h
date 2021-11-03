#include <SevenSegmentExtended.h>

#define DIO1 A0
#define CLK1 A1
#define DIO2 A2
#define CLK2 A3
#define SW1 A4
#define SW2 A5
#define BUTTON_TIME 2
#define BUTTON_SET 3
#define SEG_COLON 0b10000000

const uint16_t BLINK_DELAY = 500;

const uint8_t digitToSegment[] = {
 // XGFEDCBA
  0b00111111,    // 0
  0b00000110,    // 1
  0b01011011,    // 2
  0b01001111,    // 3
  0b01100110,    // 4
  0b01101101,    // 5
  0b01111101,    // 6
  0b00000111,    // 7
  0b01111111,    // 8
  0b01101111,    // 9
  0b01110111,    // A
  0b01111100,    // b
  0b00111001,    // C
  0b01011110,    // d
  0b01111001,    // E
  0b01110001     // F
};

const int BRIGHTNESS = 75;

int32_t player1Millis = 2 * 1000;
int32_t player2Millis = 2 * 1000;
const int32_t defaultPlayer1Millis = player1Millis;
const int32_t defaultPlayer2Millis = player2Millis;

bool player1Turn = true;
bool display1On = true;
bool display2On = true;

uint16_t prevTime = 0, deltaTime;
int prevSW1 = 0, prevSW2 = 0, prevTimeButton = 0, prevSetButton = 0;
int deltaSW1 = 0, deltaSW2 = 0, deltaTimeButton = 0, deltaSetButton = 0;

SevenSegmentExtended display1 (CLK1, DIO1);
SevenSegmentExtended display2 (CLK2, DIO2);

// All segments on
const uint8_t allOn[] = {0b11111111, 0b11111111, 0b11111111, 0b11111111};
const uint8_t allOff[] = {0,0,0,0}; // or use display.clear

void resetTimers() {
  player1Millis = defaultPlayer1Millis;
  player2Millis = defaultPlayer2Millis;
}

void toggleDisplay1(){
  display1.setBacklight(display1On ? 0 : 100);
  display1On = !display1On;
}

void toggleDisplay2(){
  display2.setBacklight(display2On ? 0 : 100);
  display2On = !display2On;
}
