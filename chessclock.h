#include <SevenSegmentExtended.h>

#define DIO1 A0
#define CLK1 A1
#define DIO2 A2
#define CLK2 A3
#define SW1 A4
#define SW2 A5
#define BUTTON_TIME 13
#define BUTTON_SET 12
#define LED1 11
#define LED2 10
#define BUZZER 9
#define SEG_COLON 0b10000000

const uint16_t BLINK_DELAY = 200;

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
const uint32_t MAX_TIME = 5999000; // 99:59
const uint8_t MAX_INC = 99; // 99 seconds

// 2 seconds with 2 seconds increment
const int32_t defaultPlayerMillis = 300000; // ms
const uint8_t defaultPlayerIncrement = 2; // seconds

int32_t player1Millis = defaultPlayerMillis;
int8_t player1Increment = defaultPlayerIncrement;
int32_t player2Millis = defaultPlayerMillis;
int8_t player2Increment = defaultPlayerIncrement;

bool player1Turn = true;
bool display1On = true;
bool display2On = true;

uint16_t prevTime = 0, deltaTime;
int prevSW1 = 0, prevSW2 = 0, prevTimeButton = 0, prevSetButton = 0;
int deltaSW1 = 0, deltaSW2 = 0, deltaTimeButton = 0, deltaSetButton = 0;

int msCounter = 0; // Used for blinking display

byte setIndex = 0; // Selector index for time and increment set functions

// Allows the selected number to flash
bool numberOn = false;

// How many milliseconds each place in mm:ss is worth
int32_t indexSignificance[] = {600000, 60000, 10000, 1000};
int16_t indexSignificanceSeconds[] = {600, 60, 10, 1};

enum gameState {
  starting_game,
  playing_game,
  setting_time,
  setting_inc,
  player1_timeout,
  player2_timeout
};
gameState state;

SevenSegmentExtended display1 (CLK1, DIO1);
SevenSegmentExtended display2 (CLK2, DIO2);

// All segments on
const uint8_t allOn[] = {0b11111111, 0b11111111, 0b11111111, 0b11111111};
const uint8_t allOff[] = {0,0,0,0}; // or use display.clear

void resetTimers() {
  player1Millis = defaultPlayerMillis;
  player2Millis = defaultPlayerMillis;
  player1Turn = true;
}

void toggleDisplay1(){
  // If on, turn off and vice versa
  display1.setBacklight(display1On ? 0 : BRIGHTNESS);
  display1On = !display1On;
}

void toggleDisplay2(){
  display2.setBacklight(display2On ? 0 : BRIGHTNESS);
  display2On = !display2On;
}

void tick() {
  noTone(BUZZER);
  deltaTime = millis() - prevTime;
  prevTime = millis();

  // deltaSW1: -1 -> button released, 0 -> no action, 1 -> button pressed
  deltaSW1 = !digitalRead(SW1) - prevSW1;
  prevSW1 = !digitalRead(SW1);

  deltaSW2 = !digitalRead(SW2) - prevSW2;
  prevSW2 = !digitalRead(SW2);

  deltaTimeButton = !digitalRead(BUTTON_TIME) - prevTimeButton;
  prevTimeButton = !digitalRead(BUTTON_TIME);

  deltaSetButton = !digitalRead(BUTTON_SET) - prevSetButton;
  prevSetButton = !digitalRead(BUTTON_SET);
  delay(1);
}


void displayTime(SevenSegmentExtended disp, uint32_t ms) {
  double seconds = ms / 1000.0;
  double minutes = seconds / 60.0;
  seconds = minutes*60 - (int)minutes*60;

  disp.home(); // Reset cursor

  if (minutes < 10){
    if (state == playing_game)
      disp.print(" ");
      
    disp.setCursor(0,1);
  }
  disp.print((int)minutes);
  
  disp.setCursor(0,2);
  if (seconds < 10){
    disp.print(0);
    disp.setCursor(0,3); 
  }
  disp.print((int)seconds);
}

// leadingZero determines if zero should be shown in tens place of minutes
void displayTime(SevenSegmentExtended disp, uint32_t ms, bool leadingZero) {
  displayTime(disp, ms);
  if(ms / 60000 < 10) { // Only if there are less than ten minutes on the clock
    disp.setCursor(0, 0);
    disp.print(0);
  }
}

// skipIndex leaves a blank space at the index
void displayTime(SevenSegmentExtended disp, uint32_t ms, byte skipIndex) {
  displayTime(disp, ms);
  disp.setCursor(0, skipIndex);
  disp.print(" ");
}

void displayIncrement(SevenSegmentExtended disp, uint8_t seconds) {
  disp.clear();

  disp.printNumber(seconds, false, false, true);
  if (seconds < 10){ // Add leading zero
    disp.setCursor(0, 2);
    disp.print(0);
  }
}

void eraseDigit(SevenSegmentExtended disp, byte eraseIndex){
  disp.setCursor(0, eraseIndex);
  disp.print(" ");
}
