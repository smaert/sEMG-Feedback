/*
 *** A LOW-COST sEMG feedback device, based on Arduino Mega and Advancer muscle sensor 'v3' 
 * 
 *  View the prototype: https://www.youtube.com/watch?v=2l5GvTqeB1E *
 * 
***NOTICE! I used a 8x2 LCD (which looks like 16x1), apply nessecary changes for an actual 16x1 LCD!!!***
*
*
*
The code is "assembled" by Märt Reinvee mart.reinvee@emu.ee, based on the following built in examples (https://www.arduino.cc/en/Tutorial/HomePage):
1) Analog Read Serial
2) Button by DojoDave & Tom Igoe
3) Tone Pitch Follower by Tom Igoe, with suggestion from Michael Flynn
4) Calibration by David A. Mellis & Tom Igoe
5) Smoothing by David A. Mellis & Tom Igoe
6) Switch Case by Tom Igoe
*
And
7) "Hello World" from LiquidCrystal Library  by Tom Igoe (library by David A. Mellis & Limor Fried)
*
*
*/

// include the library code:
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

// CONSTANTS:

//please notice, that "cal_switch" is used to start calibration squence and switch between audio and visual feedback
const int cal_switch = 2;
// sEMG sensor is connected to analog 0
const int sensorPin = A0;

// Total of 10 leds are connected to Arduino Mega, 4 - green (g) 3 yellow (y)  2 orange (o ) and 1 red (r)
const int g1 = 22;
const int g2 = 24;
const int g3 = 26;
const int g4 = 28;
const int y5 = 30;
const int y6 = 32;
const int y7 = 34;
const int o8 = 36;
const int o9 = 38;
const int r10 = 40;

//piezo speaker
const int sound = 3;
//smoothing
const int numReadings = 10;



// VARIABLES:

//button state 0/I
int buttonState = 0;  

// calibration duration
int seconds = 5;

//variables for scaling i.e the values for rest and mvc
int sensorValue = 0;         // the sensor value
int sensorMin = 1023;        // minimum sensor value
int sensorMax = 0;           // maximum sensor value

//smoothening
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average


void setup() {
 //OUTPUTS-INPUTS
pinMode(g1, OUTPUT);
pinMode(g2, OUTPUT);
pinMode(g3, OUTPUT);
pinMode(g4, OUTPUT);
pinMode(y5, OUTPUT);
pinMode(y6, OUTPUT);
pinMode(y7, OUTPUT);
pinMode(o8, OUTPUT);
pinMode(o9, OUTPUT);
pinMode(r10, OUTPUT);
pinMode(cal_switch, INPUT);

//Decide wether to run calibration or not

 buttonState = digitalRead(cal_switch);
 // set up the LCD's number of columns and rows:
 lcd.begin(8, 2); 
// initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
 //RUN WIHTOUT CALIBRATION

  if (buttonState == LOW){
      lcd.print("CAL OFF");
      while (seconds >= 0) {
            lcd.setCursor(1, 1); 
            lcd.print(seconds);
            seconds--;
            delay(1000);                        
      }
    }

  else {
  //RUN CALIBRATION SEQUENCE
       //Indicate start of calibration sequence with LEDs & on LCD
       lcd.clear();
       lcd.print("CAL ON:");
       digitalWrite(g1, HIGH);
       digitalWrite(g2, HIGH);
       digitalWrite(g3, HIGH);
       digitalWrite(g4, HIGH);
       digitalWrite(y5, HIGH);
       digitalWrite(y6, HIGH);
       digitalWrite(y7, HIGH);
       digitalWrite(o8, HIGH);
       digitalWrite(o9, HIGH);
       digitalWrite(r10, HIGH);
       delay(500);

       long t1 = millis();
       while (millis()-t1 < 10000) {
       sensorValue = analogRead(sensorPin);         
       // record the maximum sensor value
             if (sensorValue > sensorMax) {
                 sensorMax = sensorValue; }
       // record the minimum sensor value
             if (sensorValue < sensorMin) {
                 sensorMin = sensorValue; }
                       lcd.setCursor(2, 1); 
                  lcd.print((9999-millis()+t1)/1000);
                                   }
   // signal the end of the calibration period
   // flash leds
        digitalWrite(g1, LOW);
        digitalWrite(g2, LOW);
        digitalWrite(g3, LOW);
        digitalWrite(g4, LOW);
        digitalWrite(y5, LOW);
        digitalWrite(y6, LOW);
        digitalWrite(y7, LOW);
        digitalWrite(o8, LOW);
        digitalWrite(o9, LOW);
        digitalWrite(r10, LOW);
        delay(500);
       digitalWrite(g1, HIGH);
       delay(100);
       digitalWrite(g2, HIGH);
       delay(100);
       digitalWrite(g3, HIGH);
       delay(100);
       digitalWrite(g4, HIGH);
       delay(100);
       digitalWrite(y5, HIGH);
       delay(100);
       digitalWrite(y6, HIGH);
       delay(100);
       digitalWrite(y7, HIGH);
       delay(100);
       digitalWrite(o8, HIGH);
       delay(100);
       digitalWrite(o9, HIGH);
       delay(100);
       digitalWrite(r10, HIGH);
        digitalWrite(g1, LOW);
        digitalWrite(g2, LOW);
        digitalWrite(g3, LOW);
        digitalWrite(g4, LOW);
        digitalWrite(y5, LOW);
        digitalWrite(y6, LOW);
        digitalWrite(y7, LOW);
        digitalWrite(o8, LOW);
        digitalWrite(o9, LOW);
        digitalWrite(r10, LOW);      
   
        }     
  lcd.clear();  
}

void loop() {
  
//SCALE RESULTS - (i have 10 LEDs / write numeric results to LCD with resolution of 10% MVC)
  sensorValue = analogRead(sensorPin);
  sensorValue = map(sensorValue, sensorMin, sensorMax, 0, 10);

//SMOOTHING:
   // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = sensorValue;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;
  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }
  // calculate the average:
  average = total / numReadings;
  digitalRead(cal_switch);
  buttonState = digitalRead(cal_switch);
  
 //READ SWITCH and provide audio or feedback if buttonstate is LOW.. 
 if (buttonState == LOW){
  int thisPitch = map(average, 0, 10, 120, 1500);
  //tone(pin, frequency, duration)
 tone(sound, thisPitch, 10);
 }
 else{
//IF buttonstate is HIGH, provide visual feedback with LCD and LEDs
 switch (average) {
    case 0:    // 0% - rest
      lcd.setCursor(0, 1); 
      lcd.print("<10% MVC");
        digitalWrite(g1, HIGH);
        digitalWrite(g2, LOW);
        digitalWrite(g3, LOW);
        digitalWrite(g4, LOW);
        digitalWrite(y5, LOW);
        digitalWrite(y6, LOW);
        digitalWrite(y7, LOW);
        digitalWrite(o8, LOW);
        digitalWrite(o9, LOW);
        digitalWrite(r10, LOW);
      break;
    case 1:    
      lcd.setCursor(0, 1); 
      lcd.print(">10% MVC");
        digitalWrite(g1, HIGH);
        digitalWrite(g2, HIGH);
        digitalWrite(g3, LOW);
        digitalWrite(g4, LOW);
        digitalWrite(y5, LOW);
        digitalWrite(y6, LOW);
        digitalWrite(y7, LOW);
        digitalWrite(o8, LOW);
        digitalWrite(o9, LOW);
        digitalWrite(r10, LOW);
      break;
    case 2:    
      lcd.setCursor(0, 1); 
      lcd.print(">20% MVC");
        digitalWrite(g1, HIGH);
        digitalWrite(g2, HIGH);
        digitalWrite(g3, HIGH);
        digitalWrite(g4, LOW);
        digitalWrite(y5, LOW);
        digitalWrite(y6, LOW);
        digitalWrite(y7, LOW);
        digitalWrite(o8, LOW);
        digitalWrite(o9, LOW);
        digitalWrite(r10, LOW);
      break;
    case 3:    
      lcd.setCursor(0, 1);
      lcd.print(">30% MVC");
        digitalWrite(g1, HIGH);
        digitalWrite(g2, HIGH);
        digitalWrite(g3, HIGH);
        digitalWrite(g4, HIGH);
        digitalWrite(y5, LOW);
        digitalWrite(y6, LOW);
        digitalWrite(y7, LOW);
        digitalWrite(o8, LOW);
        digitalWrite(o9, LOW);
        digitalWrite(r10, LOW);
      break;
    case 4:    
      lcd.setCursor(0, 1);
    lcd.print(">40% MVC");
        digitalWrite(g1, HIGH);
        digitalWrite(g2, HIGH);
        digitalWrite(g3, HIGH);
        digitalWrite(g4, HIGH);
        digitalWrite(y5, HIGH);
        digitalWrite(y6, LOW);
        digitalWrite(y7, LOW);
        digitalWrite(o8, LOW);
        digitalWrite(o9, LOW);
        digitalWrite(r10, LOW);
      break;
    case 5:    
      lcd.setCursor(0, 1);
    lcd.print(">50% MVC");
        digitalWrite(g1, HIGH);
        digitalWrite(g2, HIGH);
        digitalWrite(g3, HIGH);
        digitalWrite(g4, HIGH);
        digitalWrite(y5, HIGH);
        digitalWrite(y6, HIGH);
        digitalWrite(y7, LOW);
        digitalWrite(o8, LOW);
        digitalWrite(o9, LOW);
        digitalWrite(r10, LOW);
      break; 
    case 6:    
      lcd.setCursor(0, 1);
    lcd.print(">60% MVC");
        digitalWrite(g1, HIGH);
        digitalWrite(g2, HIGH);
        digitalWrite(g3, HIGH);
        digitalWrite(g4, HIGH);
        digitalWrite(y5, HIGH);
        digitalWrite(y6, HIGH);
        digitalWrite(y7, HIGH);
        digitalWrite(o8, LOW);
        digitalWrite(o9, LOW);
        digitalWrite(r10, LOW);
      break;
    case 7:    
      lcd.setCursor(0, 1);
    lcd.print(">70% MVC");
        digitalWrite(g1, HIGH);
        digitalWrite(g2, HIGH);
        digitalWrite(g3, HIGH);
        digitalWrite(g4, HIGH);
        digitalWrite(y5, HIGH);
        digitalWrite(y6, HIGH);
        digitalWrite(y7, HIGH);
        digitalWrite(o8, LOW);
        digitalWrite(o9, LOW);
        digitalWrite(r10, LOW);
      break;   
    case 8:    
      lcd.setCursor(0, 1);
    lcd.print(">80% MVC");
         digitalWrite(g1, HIGH);
        digitalWrite(g2, HIGH);
        digitalWrite(g3, HIGH);
        digitalWrite(g4, HIGH);
        digitalWrite(y5, HIGH);
        digitalWrite(y6, HIGH);
        digitalWrite(y7, HIGH);
        digitalWrite(o8, HIGH);
        digitalWrite(o9, LOW);
        digitalWrite(r10, LOW);     
      break;   
    case 9:   
      lcd.setCursor(0, 1);
    lcd.print(">90% MVC");
        digitalWrite(g1, HIGH);
        digitalWrite(g2, HIGH);
        digitalWrite(g3, HIGH);
        digitalWrite(g4, HIGH);
        digitalWrite(y5, HIGH);
        digitalWrite(y6, HIGH);
        digitalWrite(y7, HIGH);
        digitalWrite(o8, HIGH);
        digitalWrite(o9, HIGH);
        digitalWrite(r10, LOW);   
      break;  
   case 10:    // 100% ehk max value
      lcd.setCursor(0, 1);
    lcd.print("100% MVC");
        digitalWrite(g1, HIGH);
        digitalWrite(g2, HIGH);
        digitalWrite(g3, HIGH);
        digitalWrite(g4, HIGH);
        digitalWrite(y5, HIGH);
        digitalWrite(y6, HIGH);
        digitalWrite(y7, HIGH);
        digitalWrite(o8, HIGH);
        digitalWrite(o9, HIGH);
        digitalWrite(r10, HIGH);   
      break;
  }
 }
 
 // Delay for stability
delay(10);
 }
