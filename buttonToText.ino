
long rawTransmission [100];
long rawIndex = 0; //returns largest index that was 
//used to input into rawTransmission
int firstIndex;//indexes for cleaned raw transmission signal
int lastIndex;// no trailing or leading zeroes
int clean[50];
int lengthClean;
char morse[35];
int morseIndex;
char english[25];
int englishIndex;
int clock = 3;
int stopButton = 4;
int button = 2;
int buttonState = 0;
int stopButtonState;
int counter = 0;
int pastState = LOW;
const long blinkerInterval = 1000;
unsigned long previousMillis = 0;
int clockState = LOW;
unsigned long startPulse;
unsigned long endPulse;
unsigned long pressedTime;

//LCD
// C++ code
//
/*
  LiquidCrystal Library - Hello World

   Demonstrates the use of a 16x2 LCD display.
  The LiquidCrystal library works with all LCD
  displays that are compatible with the  Hitachi
  HD44780 driver. There are many of them out
  there, and you  can usually tell them by the
  16-pin interface.

  This sketch prints "Hello World!" to the LCD
  and shows the time.

  The circuit:
  * LCD RS pin to digital pin 13 (controls where in its memory
  you are writing data to)
  * LCD Enable pin to digital pin 12 (enables writing to 
  registers)
  Data pins are the bits you write to registers
  * LCD D4 pin to digital pin 11
  * LCD D5 pin to digital pin 10
  * LCD D6 pin to digital pin 9
  * LCD D7 pin to digital pin 8
  * LCD R/W pin to ground (selects reading or writing mode)
  * LCD VSS pin to ground
  * LCD VCC pin to 5V
  * 10K resistor:
  * ends to +5V and ground
  * wiper to LCD VO pin (pin 3) (display contrast pin)

  Library originally added 18 Apr 2008  by David
  A. Mellis
  library modified 5 Jul 2009  by Limor Fried
  (http://www.ladyada.net)
  example added 9 Jul 2009  by Tom Igoe
  modified 22 Nov 2010  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/LiquidCrystal
*/

#include <LiquidCrystal.h>
 LiquidCrystal lcd_1(13, 12, 11, 10, 9, 8);


//PLAN

//Have two time limits for each pulse, between the -250 and 1250
//after each cycle, check if button has been pressed between those limits
// if yes print1, if no print 0. If pressed time is not between current limits
//then must output zero. Need to do this after the endPulse so I have my current limits


// The following variables are unsigned longs because the time, measured in 
// milliseconds, will quickly become a bigger number that can be stored in an int
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  // Initializes serial communication at 9600 bauds per second
  Serial.begin(9600); 
  pinMode(button, INPUT); // Set pin 2 as input
  pinMode(clock,OUTPUT); // Set pin 3 as output
  pinMode(stopButton,INPUT); // Set pin 4 as input
  lcd_1.begin(16, 2); // Set up the number of columns and rows on the LCD.

  // Print a message to the LCD.
  lcd_1.print("hello world!");

}

void loop() {
  // Main code here, runs repeatedly
  stopButtonState = digitalRead(stopButton);
  if (stopButtonState == HIGH){
    for (int i = 0;i<rawIndex;i++){
      Serial.print(rawTransmission[i]);
    }
    Serial.println();
 	Serial.println(rawIndex);//only for debugging shows raw length
    cleanTransmission();
    Serial.println();
    Serial.println(lengthClean);
    cleanToMorse();
    morseToEnglish();
    lcd_1.clear();
    lcd_1.print(english);
    delay(1000000);     
  }
  buttonPress();
  blinkingClock(); 
}



void cleanToMorse() {
  morseIndex = 0;
  for (int i = 0; i < lengthClean; i++) {
    if ((i + 1 < lengthClean) && (clean[i] == 1) && (clean[i + 1] == 0)) {
      morse[morseIndex++] = '.';
      i++; // Move past the zero
    } 
    else if ((i + 3 < lengthClean) && (clean[i] == 1) && (clean[i + 1] == 1) && (clean[i + 2] == 1) && (clean[i + 3] == 0)) {
      morse[morseIndex++] = '-';
      i += 3; // Move past the three ones and zero
    } 
    else if ((i + 1 < lengthClean) && (clean[i] == 0) && (clean[i + 1] == 0)) {
      morse[morseIndex++] = ' ';
      i++; // Move past the zero
    } 
    else if ((i + 3 < lengthClean) && (clean[i] == 0) && (clean[i + 1] == 0) && (clean[i + 2] == 0) && (clean[i + 3] == 0)) {
      morse[morseIndex++] = '/';
      i += 3; // Move past the three zeros
    }
  }
  morse[morseIndex] = '\0'; // Null-terminate the string
  //is used to mark the end of a string
  Serial.println(morse);
}
void morseToEnglish() {
  // uses binary tree data structure of morse code to traverse
  // the tree and translate
  englishIndex = 0;
  int pointer = 0;
  int dash[27] = {20,23,0,0,24,1,0,17,0,21,0,25,0,15,11,0,0,0,0,22,13,0,0,10,0,0,0};
  int dot[27] = {5,18,0,0,2,9,0,26,0,19,0,3,0,7,4,0,0,0,12,8,14,6,0,16,0,0,0};
  char letter[27] = {' ','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
  char morseCode[27] = {' ','.-','-...','-.-.','-..','.','..-.','--.','....','..','.---','-.-','.-..','--','-.','---','.--.','--.-','.-.','...','-','..-','...-','.--','-..-','-.--','--..'};
  for (int i = 0;i < (morseIndex +1); i++) {
    // if morse[i] = space, then
    char symbol = morse[i];
    if (symbol == '-') {
      pointer = dash[pointer];
      }
    else if (symbol == '.') {
      pointer = dot[pointer];
    }
    else if (symbol == ' ') {
      english[englishIndex] = letter[pointer];
      englishIndex ++;
      pointer = 0;
    }
    else if (symbol == '/') {
      english[englishIndex] = letter[pointer];
      englishIndex ++;
    }
    }
  english[englishIndex] = letter[pointer];
  englishIndex ++;
  for (int j=0; j < (englishIndex+1); j++) {
    Serial.print(english[j]);
  }
}
void cleanTransmission() {
  //Plan: use backward and forward for loops checking for the 
  // last and first 1. once they have been reache, create a new
  //list just for those numbers
  // firstIndex must be less than lastIndex
  if (rawIndex > 0){
    for (int i=0;i<rawIndex;i++){
      if (rawTransmission[i] == 1){
        firstIndex = i;
        break;
      }
    }
    for (int j = rawIndex-1;j>-1;j--){
      // each symbol e.g. 10 or 1110
      if (rawTransmission[j] == 1){
        lastIndex = j+1;//+1so i dont lose the 0 after
        break;
      }
    }
    //actually printing the signal of clean (transmission)
    if (firstIndex<=lastIndex){
      for (int k=0;k<lastIndex+1-firstIndex;k++){
        clean[k] = rawTransmission[k+firstIndex];
        lengthClean ++;
       	Serial.print(rawTransmission[k+firstIndex]);
        
      }
    }
  }
}
void blinkingClock() {
  // had to use if statement instead of while loop
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= blinkerInterval) {
    previousMillis = currentMillis;

    clockState = !clockState;
    digitalWrite(clock,clockState);
    if (clockState == HIGH) {
    	startPulse = millis();
    }
    else{
      endPulse = millis();
      if ((pressedTime >= endPulse)||(pressedTime<=startPulse)){
        Serial.println(0);
        rawTransmission[rawIndex] = 0;
        rawIndex++;
      }
    }
  }
}
void buttonPress() {
  int reading = digitalRead(button);
  // If the switch changed, due to noise or pressing
  if (reading != pastState) {
    // Reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Whatever the reading is at, it's been there for longer than the debounce delay
    // So take it as the actual current state
    if (reading != buttonState) {
      buttonState = reading;
      // Only count the press when the button state changes to HIGH
      if ((buttonState == HIGH) && (clockState==HIGH)){
        pressedTime = millis();
        Serial.println(1);
        rawTransmission[rawIndex] = 1;
        rawIndex++;
      }
    }
  }
  // Save the reading. Next time through the loop, it'll be the lastButtonState
  pastState = reading;
}

