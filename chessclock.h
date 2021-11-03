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

const uint16_t BLINK_DELAY = 333;

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

int msCounter = 0; // Used for blinking display

enum gameState {
  starting_game,
  playing_game,
  setting_time,
  setting_inc,
  player1_timeout,
  player2_timeout
};

SevenSegmentExtended display1 (CLK1, DIO1);
SevenSegmentExtended display2 (CLK2, DIO2);

// All segments on
const uint8_t allOn[] = {0b11111111, 0b11111111, 0b11111111, 0b11111111};
const uint8_t allOff[] = {0,0,0,0}; // or use display.clear

void resetTimers() {
  player1Millis = defaultPlayer1Millis;
  player2Millis = defaultPlayer2Millis;
  player1Turn = true;
}

void toggleDisplay1(){
  display1.setBacklight(display1On ? 0 : 100);
  display1On = !display1On;
}

void toggleDisplay2(){
  display2.setBacklight(display2On ? 0 : 100);
  display2On = !display2On;
}

void tick() {
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

  disp.clear();
  disp.home(); // Reset cursor

  if (minutes < 10)
    disp.setCursor(0,1);
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
  if(ms / (1000*60) < 10) { // Only if there are less than ten minutes on the clock
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
