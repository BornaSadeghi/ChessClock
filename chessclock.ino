#include "chessclock.h"


void setup() {
  Serial.begin(9600);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(BUTTON_TIME, INPUT_PULLUP);
  pinMode(BUTTON_SET, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUZZER, OUTPUT);
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

  if (deltaSW1 || deltaSW2 || deltaTimeButton || deltaSetButton){
    Serial.println("deltaSW1, deltaSW2, deltaTimeButton, deltaSetButton:");
    Serial.print(deltaSW1); Serial.print(" "); Serial.print(deltaSW2); Serial.print(" ");
    Serial.print(deltaTimeButton); Serial.print(" "); Serial.println(deltaSetButton); 
    tone(BUZZER, 440, 20);
  }

  
  if (state == starting_game){
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    // Wait for black to start white's timer.
    displayTime(display1, player1Millis);
    display1.setColonOn(true);
    display2.setColonOn(false);
    display2.print("PLAY");
    
    if (deltaSW2 == 1) { // If black presses sw2
      player1Turn = true;
      display1.clear();
      display2.clear();
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      display2.setColonOn(true);
      display1.setBacklight(BRIGHTNESS); // Ensure display1 is on
      state = playing_game;
    } else if (deltaTimeButton == 1) { // If time button pressed
      display1.clear();
      display2.clear();
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

    if (player1Turn && deltaSW1 == 1){ // Player 1 ends turn
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      player1Turn = !player1Turn;
      player1Millis += player1Increment * 1000;
    } else if (!player1Turn && deltaSW2 == 1){ // Player 2 ends turn
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      player1Turn = !player1Turn;
      player2Millis += player2Increment * 1000;
    } else if (deltaTimeButton == 1) { // Stop game and set time
      display1.clear();
      display2.clear();
      displayTime(display1, player1Millis);
      displayTime(display2, player2Millis);
      
      setIndex = 0;
      state = setting_time;
    }
  
    if (player1Turn) {
      player1Millis -= deltaTime;
      if (player1Millis <= 0){
        display1.clear();
        state = player1_timeout;
      }
    } else {
      player2Millis -= deltaTime;
      if (player2Millis <= 0){
        display2.clear();
        state = player2_timeout;
      }
    }


    
  } else if (state == setting_time) {
    display1.setColonOn(true);
    display2.setColonOn(true);
    
    // Iterate through and set all digits with setIndex
    if (setIndex < 4){ // Player 1's digits
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      if (deltaSW1 == 1){ // SW1 button decrements the value
        player1Millis -= indexSignificance[setIndex%4];
        player1Millis = max(1000, player1Millis);
      }
      if (deltaSW2 == 1 && player1Millis+indexSignificance[setIndex%4] < MAX_TIME){ // SW2 button increments the value
        player1Millis += indexSignificance[setIndex%4];
      }
      if (deltaSetButton == 1) { // Set button moves to next number
        setIndex++;
        numberOn = true; // Make sure number is left on
      }

      if (numberOn) {
        displayTime(display1, player1Millis, true);
      } else {
        eraseDigit(display1, setIndex);
      }
      
    } else if (setIndex >= 4 && setIndex < 8) { // Player 2's digits
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      if (deltaSW1 == 1){ // SW1 button decrements the value
        player2Millis -= indexSignificance[setIndex-4];
        player2Millis = max(1000, player2Millis);
      }
      if (deltaSW2 == 1 && player2Millis+indexSignificance[setIndex%4] < MAX_TIME){ // SW2 button increments the value
        player2Millis += indexSignificance[setIndex-4];
      }
      if (deltaSetButton == 1) { // Set button moves to next number
        setIndex++;
        numberOn = true; // Make sure number is left on
      }

      if (numberOn) {
        displayTime(display2, player2Millis, true);
      } else {
        eraseDigit(display2, setIndex-4);
      }
    } else { // Finish set time and go to set increment
      setIndex = 2;
      display1.clear();
      display2.clear();
      display1.setColonOn(false);
      display2.setColonOn(false);
      displayIncrement(display1, player1Increment);
      displayIncrement(display2, player2Increment);
      state = setting_inc;
    }
    if (msCounter >= BLINK_DELAY) {
      numberOn = !numberOn;
      msCounter = 0;
    }
    msCounter += deltaTime;



    
  } else if (state == setting_inc) {
    
    Serial.print(deltaSetButton); Serial.print(" "); Serial.println(setIndex);
    if (setIndex < 4){ // player 1 increment
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      if (deltaSW1 == 1){ // SW1 button decrements the value
        player1Increment -= indexSignificanceSeconds[setIndex%4];
        player1Increment = max(0, player1Increment);
      }
      if (deltaSW2 == 1 && player1Increment+indexSignificanceSeconds[setIndex%4] < MAX_INC){ // SW2 button increments the value
        player1Increment += indexSignificanceSeconds[setIndex%4];
      }
      if (deltaSetButton == 1) { // Set button moves to next number
        setIndex++;
        numberOn = true; // Make sure number is left on
        if (setIndex == 4){ // skip directly to 10 seconds place on display 2
          setIndex = 6;
        }
      }
      if (numberOn) {
        displayIncrement(display1, player1Increment);
      } else {
        eraseDigit(display1, setIndex);
      } 
      
    } else if (setIndex < 8) { // player 2 increment
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      if (deltaSW1 == 1){ // SW1 button decrements the value
        player2Increment -= indexSignificanceSeconds[setIndex%4];
        player2Increment = max(0, player2Increment);
      }
      if (deltaSW2 == 1 && player2Increment+indexSignificanceSeconds[setIndex%4] < MAX_INC){ // SW2 button increments the value
        player2Increment += indexSignificanceSeconds[setIndex%4];
      }
      if (deltaSetButton == 1) { // Set button moves to next number
        setIndex++;
        numberOn = true; // Make sure number is left on
      }
      if (numberOn) {
        displayIncrement(display2, player2Increment);
      } else {
        eraseDigit(display2, setIndex-4);
      }
      
    } else {
      state = starting_game;
    }

    if (msCounter >= BLINK_DELAY) {
      numberOn = !numberOn;
      msCounter = 0;
    }
    msCounter += deltaTime;
    

  } else if (state == player1_timeout) {
    display1.setColonOn(false);
    display1.print("LOSE");

    // Wait for input
    if (deltaSW1 == 1 || deltaSW2 == 1) {
      display2.setBacklight(BRIGHTNESS); // Turn on display in case it's off
      display1.clear();
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
      display2.clear();
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
