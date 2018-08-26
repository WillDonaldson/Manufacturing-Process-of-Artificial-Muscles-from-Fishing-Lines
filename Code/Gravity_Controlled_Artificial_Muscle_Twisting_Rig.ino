/*
 * William Donaldson 2018
 * Code for a rig that twists nylon fishing lines to create artificial muscles
 */

#include "HX711.h"
#include <LiquidCrystal.h>

int calibration_factor=210; //Found experimentally by comparing to known weights
#define CLK  13
#define DOUT 12
HX711 scale(DOUT, CLK);

const int rs = 22, en = 24, d4 = 26, d5 = 28, d6 = 30, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int button1=20, button2=19;
const int potPin=A0;

const int B1A=23, B1B=25;

bool spin=false;
int spd;
bool Exit=false;

const int IRdetector=2;
volatile byte irCount=0;
int revCount=0;
float rpm=0.0;
int prevTime;
int NumberOfBlades=45; 

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);

  scale.set_scale(calibration_factor); 
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

  pinMode(button1, INPUT);
  pinMode(button2, INPUT);

  pinMode(potPin, INPUT);

  pinMode(B1A, OUTPUT);
  pinMode(B1B, OUTPUT);

  pinMode(IRdetector, INPUT);
  attachInterrupt(digitalPinToInterrupt(IRdetector), counter, CHANGE); 
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("1. Measure Force");
  lcd.setCursor(0, 1);
  lcd.print("2. Insert Twist");
  
  if(digitalRead(button1)==HIGH){
    force();
  }

  if(digitalRead(button2)==HIGH){
    twist();
  }
  delay(10);
}

void twist(){
  lcd.clear();
  lcd.print("1. Start/pause");
  lcd.setCursor(0,1);
  lcd.print("2. Back");
  delay(500);
  while(digitalRead(button1)==LOW &&digitalRead(button2)==LOW){ //wait for instructions
    delay(5);
  }
  if(digitalRead(button1)==HIGH){
      spin=true;
      revCount=0;  //reset from previous run
      delay(500);
      lcd.clear();
      lcd.print("Twisting...");
      lcd.setCursor(0,1);
      lcd.print("press 1 to pause");
  }

  if(digitalRead(button2)==HIGH){
      spin=false;
      delay(300);
  }
  if(spin==true){
    while(spin==true){
        //spd=map(analogRead(potPin), 0, 1023, 0, 255); 
        spd=160;  //since line above doesn't vary speed much 
        analogWrite(B1A, 0);
        analogWrite(B1B, spd);
        if(irCount>(NumberOfBlades*2)){
          revCount+=1;
          irCount=0;
          Serial.println(revCount);
        }
        
        if(digitalRead(button1)==HIGH){ //pause
          delay(300);
          analogWrite(B1A, 0);
          analogWrite(B1B, 0);
          lcd.clear();
          lcd.print("Paused");
          lcd.setCursor(0,1);
          lcd.print("1=restart 2=stop");
          while(digitalRead(button1)==LOW &&digitalRead(button2)==LOW){ //wait for instructions
            delay(20);
          }
          if(digitalRead(button1)==HIGH){  //resume twisting
            delay(300);
            lcd.clear();
            lcd.print("Twisting...");
            lcd.setCursor(0,1);
            lcd.print("press 1 to pause");
            //do nothing, return to twisting
          }
          if(digitalRead(button2)==HIGH){   //stop twisting completely
            delay(300);
            spin=false; //stop twisting
          }
        }
      }
    lcd.clear();
    lcd.print("# of revs:");
    lcd.print(revCount);
    delay(1000);
    lcd.setCursor(0,1);
    lcd.print("Press 2 to exit");
    while(digitalRead(button2)==LOW){ //wait for instructions
      delay(20);
    }
    delay(300);
  }
}

void counter(){
  irCount++;  
}

void force(){
  lcd.clear();
  lcd.print("1. Calibrate");
  lcd.setCursor(0, 1);
  lcd.print("2. Measure Force");
  delay(300);
  while(digitalRead(button1)==LOW &&digitalRead(button2)==LOW){ //wait for instructions
    delay(5);
  }

  if(digitalRead(button1)==HIGH){
    lcd.clear();
    lcd.print("Hang known mass"); //hang known weight as a counterweight and find calibration offset
    lcd.setCursor(0,1);
    lcd.print("Tune with pot"); //change calibation factor with potentiometer
    delay(3000);
    while(digitalRead(button2)==LOW){   //Need to press button to exit loop
      lcd.setCursor(0,0);
      lcd.print("Measured: ");
      lcd.print((scale.get_units()/1000.0)*9.81, 0);
      lcd.print("g");
      lcd.setCursor(0,1);
      lcd.print("Offset: ");
      calibration_factor=analogRead(potPin);
      lcd.print(calibration_factor);
      scale.set_scale(calibration_factor);
      delay(100);
      lcd.clear();
    }
    delay(500);
    
  }
  
  if(digitalRead(button2)==HIGH){
    lcd.clear();
    delay(200);
    while(digitalRead(button2)==LOW){ //wait for instructions
      lcd.setCursor(0,0);
      lcd.print("Force: ");
      lcd.print((scale.get_units()/100000.0)*9.81, 3);
      lcd.print("N");
      lcd.setCursor(0,1);
      lcd.print("Offset: ");
      lcd.print(calibration_factor);
      delay(100);
    }
    delay(300);
  }

}




