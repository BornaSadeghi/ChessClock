#include "chessclock.h"

uint16_t prevTime = 0, deltaTime;

void tick() {
  deltaTime = millis() - prevTime;
  prevTime = millis();
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
  while (digitalRead(SW2)) {
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
    display1.print("LOSE");
    display1.setColonOn(false);
  } else {
    display2.print("LOSE");
    display2.setColonOn(false);
  }
  int msCounter = 0;
  while (true) {
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
  }
  resetTimers();
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

  delay(1000);

  display1.clear();
  display2.clear();
}

void loop() {
//  // put your main code here, to run repeatedly:
//  display1.showNumberDecEx(1234, 0b01000000, false, 4, 0);
//  display2.showNumberDecEx(1234, 0b01000000, false, 4, 0);
//  delay(200);
//  display1.clear();
//  display2.clear();
//  delay(200);
  
//  display1.showNumberDecEx(millisToTime(player1Millis), 0b01000000);
//  display2.showNumberDecEx(millisToTime(player2Millis), 0b01000000);
  init_game();

  // Game loop
  while (true) { 
    displayTime(display1, player1Millis);
    displayTime(display2, player2Millis);

    if (player1Turn && !digitalRead(SW1) || !player1Turn && !digitalRead(SW2)){
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
  
    delay(1);
  }
}
