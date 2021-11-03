#include "chessclock.h"

void timeSet() {

  displayTime(display1, player1Millis, true);
  displayTime(display2, player2Millis, true);
  
  // Iterate through and set all digits
  int msCounter = 0;
  bool numberOn = false;
  tick();
  while (deltaSetButton != 1) {
    tick();
    if (msCounter >= BLINK_DELAY){
      if (numberOn) {
        displayTime(display1, player1Millis, true);
      } else {
        displayTime(display1, player1Millis, (byte)0);
      }
      numberOn = !numberOn;
      msCounter = 0;
    }
    msCounter += deltaTime;
  }
}

gameState state;


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

  state = starting_game;
}

void loop() {
  tick();
  if (deltaSW1 != 0 || deltaSW2 != 0){
    Serial.println("deltaSW1, deltaSW2, deltaTimeButton, deltaSetButton:");
    Serial.print(deltaSW1); Serial.print(" "); Serial.print(deltaSW2); Serial.print(" ");
    Serial.print(deltaTimeButton); Serial.print(" "); Serial.println(deltaSetButton); 
  }

  
  if (state == starting_game){
    
    // Wait for black to start white's timer.
    displayTime(display1, player1Millis);
    display1.setColonOn(true);
    display2.setColonOn(false);
    display2.print("PLAY");
    
    if (deltaSW2 == 1) { // If black presses sw2
      state = playing_game;
      display2.setColonOn(true);
      display1.setBacklight(BRIGHTNESS); // Ensure display1 is on
    } else if (deltaTimeButton == 1) { // If time button pressed
      display1.setBacklight(BRIGHTNESS); // Ensure displays are on
      display2.setBacklight(BRIGHTNESS);
      display1.setColonOn(true);
      display2.setColonOn(true);
      displayTime(display1, player1Millis, true);
      displayTime(display2, player1Millis, true);
      setIndex = 0; // Start from the first number on display1

      state = setting_time;
    } else { // If nothing happens, blink white's display
      msCounter += deltaTime;
      if (msCounter >= BLINK_DELAY) {
        toggleDisplay1();
        msCounter = 0;
      }
    }




  } else if (state == playing_game) {


    
    displayTime(display1, player1Millis);
    displayTime(display2, player2Millis);

    if (player1Turn && deltaSW1 == 1 || !player1Turn && deltaSW2 == 1){
      player1Turn = !player1Turn;
    }
  
    if (player1Turn) {
      player1Millis -= deltaTime;
      if (player1Millis <= 0){
        state = player1_timeout;
      }
    } else {
      player2Millis -= deltaTime;
      if (player2Millis <= 0){
        state = player2_timeout;
      }
    }


    
  } else if (state == setting_time) {
    display1.setColonOn(true);
    display2.setColonOn(true);
    
    // Iterate through and set all digits with setIndex
    if (setIndex < 4){ // Player 1's digits
      Serial.print(setIndex%4); Serial.print(" "); Serial.println(player1Millis);
      if (deltaSW1 == 1){ // SW1 button decrements the value
        player1Millis -= indexSignificance[setIndex%4];
      }
      if (deltaSW2 == 1){ // SW2 button increments the value
        player1Millis += indexSignificance[setIndex%4];
      }
      if (deltaSetButton == 1) { // Set button moves to next number
        setIndex++;
      }

      if (numberOn) {
        displayTime(display1, player1Millis, true);
      } else {
        eraseDigit(display1, setIndex);
      }
      
    } else if (setIndex >= 4 && setIndex < 8) { // Player 2's digits
      if (deltaSW1 == 1){ // SW1 button decrements the value
        player2Millis -= indexSignificance[setIndex-4];
      }
      if (deltaSW2 == 1){ // SW2 button increments the value
        player2Millis += indexSignificance[setIndex-4];
      }
      if (deltaSetButton == 1) { // Set button moves to next number
        setIndex++;
      }

      if (numberOn) {
        displayTime(display2, player2Millis, true);
      } else {
        eraseDigit(display2, setIndex-4);
      }
    } else { // Finish set time and go to set increment
      setIndex = 0;
//      state = setting_inc;
      state = starting_game;
    }
    


    if (msCounter >= BLINK_DELAY) {
      numberOn = !numberOn;
      msCounter = 0;
    }
    msCounter += deltaTime;



    
  } else if (state == setting_inc) {


    
  } else if (state == player1_timeout) {
    display1.setColonOn(false);
    display1.print("LOSE");

    // Wait for input
    if (deltaSW1 == 1 || deltaSW2 == 1) {
      display2.setBacklight(BRIGHTNESS); // Turn on display in case it's off
      state = starting_game;
    }

    msCounter += deltaTime;
    if (msCounter >= BLINK_DELAY) {
      toggleDisplay1();
      msCounter = 0;
    }
    resetTimers();




  } else if (state == player2_timeout) {
    display2.setColonOn(false);
    display2.print("LOSE");

    // Wait for input
    if (deltaSW1 == 1 || deltaSW2 == 1) {
      display2.setBacklight(BRIGHTNESS); // Turn on display in case it's off
      state = starting_game;
    }

    msCounter += deltaTime;
    if (msCounter >= BLINK_DELAY) {
      toggleDisplay2();
      msCounter = 0;
    }
    resetTimers();
  }
}
