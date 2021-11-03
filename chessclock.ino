#include "chessclock.h"

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


void init_game() {
  // Wait for black to start white's timer.
  display1.setColonOn(true);
  displayTime(display1, player1Millis);
  display2.print("PLAY");
  
  int msCounter = 0;
  while (deltaSW2 != 1) {
    msCounter += deltaTime;
    if (msCounter >= BLINK_DELAY) {
      toggleDisplay1();
      msCounter = 0;
    }
    tick();
  }

  display2.setColonOn(true);
  display1.setBacklight(BRIGHTNESS);
}


void game_over(bool player1Timeout) {
  Serial.println("Game over");
  // Wait for input
  if (player1Timeout){
    display1.setColonOn(false);
    display1.print("LOSE");
  } else {
    display2.setColonOn(false);
    display2.print("LOSE");
  }
  int msCounter = 0;
  while (deltaSW1 != 1 && deltaSW2 != 1) {
    msCounter += deltaTime;
    if (msCounter >= BLINK_DELAY) {
      if (player1Timeout){
        toggleDisplay1();
      } else {
        toggleDisplay2();
      }
      msCounter = 0;
    }
    tick();
    Serial.println(deltaSW2);
  }
  display2.setColonOn(false);
  resetTimers();
  tick();
  Serial.println(deltaSW2);
}


void setup() {
  Serial.begin(9600);
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(BUTTON_TIME, INPUT);
  pinMode(BUTTON_SET, INPUT);
  display1.init();
  display2.init();

  display1.setBacklight(BRIGHTNESS);
  display2.setBacklight(BRIGHTNESS);

  display1.printRaw(allOn, 4, 0);
  display2.printRaw(allOn, 4, 0);

  delay(500);

  display1.clear();
  display2.clear();
}


void loop() {
  init_game();

  // Game loop
  while (true) { 
    displayTime(display1, player1Millis);
    displayTime(display2, player2Millis);

    if (player1Turn && deltaSW1 == 1 || !player1Turn && deltaSW2 == 1){
      player1Turn = !player1Turn;
    }
  
    tick();
    if (player1Turn) {
      player1Millis -= deltaTime;
      if (player1Millis <= 0){
        game_over(true);
        break;
      }
    } else {
      player2Millis -= deltaTime;
      if (player2Millis <= 0){
        game_over(false);
        break; 
      }
    }
  }
  tick();
}
