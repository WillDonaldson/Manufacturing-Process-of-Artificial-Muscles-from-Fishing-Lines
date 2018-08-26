/*
 * References:
 * Load Cell Driver: https://learn.sparkfun.com/tutorials/load-cell-amplifier-hx711-breakout-hookup-guide
 * 
 */



#include "HX711.h"
#define calibration_factor 210.0 //Found experimentally by comparing to known weights
#define CLK  2
#define DOUT 3
HX711 scale(DOUT, CLK);
float setpointForce=0.40; //Newtons
float force, error;
float K_d, K_i, K_p;  //PID loop constants

#define MinLengthSwitch 18
#define MaxLengthSwitch 19
volatile byte MinSwitchState=1;
volatile byte MaxSwitchState=1;

#define IRdetector 21
volatile byte rpmCount=0;
int rpm=0, prevTime;
int NumberOfBlades=45; 

#define B1A 5
#define B1B 6
int DCspeed=70;

#define stepPin 9
#define dirPin 8
int stepsPerRev=200;
int stepperSpd=200; // [0,255]
int timeDelay= 256-stepperSpd;     //time delay (millis) between each step
int stepperRPM=(60*1000/stepperSpd)/stepsPerRev;
int stepperDirection=1;
bool switchTriggered=false;

#include <AccelStepper.h>

AccelStepper stepper(1, 9, 8); // pin 9 = step, pin 8 = direction


void setup() {
  Serial.begin(9600);

  scale.set_scale(calibration_factor); 
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0


  pinMode(IRdetector, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IRdetector), counter, FALLING); //trigger when pins 'falls' from high to low
  pinMode(MinLengthSwitch, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MinLengthSwitch), minSwitchTrigger, FALLING); 
  pinMode(MaxLengthSwitch, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MaxLengthSwitch), maxSwitchTrigger, FALLING); 
  pinMode(B1A, OUTPUT);
  pinMode(B1B, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  stepper.setMaxSpeed(500);
  

  prevTime=0;
}

void loop() {
  //stepperMove(stepperDirection, stepperSpd);
  
  
  clockwiseDC();
  
  
  /*
  delay(10);
  Serial.print(digitalRead(IRdetector));
  Serial.print("     ");
  noInterrupts(); //protect time-senstive calculations
  rpm=(60*1000/(millis()-prevTime)*rpmCount)/NumberOfBlades;
  prevTime=millis();
  rpmCount=0;
  interrupts(); //re-enable interupts 
  
  Serial.print(rpm);
  */

  
  /*
  MinSwitchState=digitalRead(MinLengthSwitch);
  MaxSwitchState=digitalRead(MaxLengthSwitch);
  */


  /*
  clockwiseDC();
  Serial.print(MinSwitchState);
  Serial.print(MaxSwitchState);
  while(switchTriggered==true){
    stopDC();
    switchTriggered=false;
    MinSwitchState=1;
    MinSwitchState=1;
    delay(500);
  }
  */

  
  stepperMove();
  
  
  
  Serial.print("Weight: ");
  Serial.print(scale.get_units()/10000.0, 4); //scale.get_units() returns a float
  Serial.print(" kg-f"); //You can change this to kg but you'll need to refactor the calibration_factor
  Serial.print("    Force: ");
  Serial.print((scale.get_units()/10000.0)*9.81);
  Serial.print(" Newtons");
  

  
  Serial.println();
  delay(1);
}

void counter(){
  rpmCount++;
}

void clockwiseDC(){
  analogWrite(B1A, 0);
  analogWrite(B1B, DCspeed);
}

void counterclockwiseDC(){
  analogWrite(B1A, DCspeed);
  analogWrite(B1B, 0);
}

void stopDC(){
  analogWrite(B1A, 0);
  analogWrite(B1B, 0);
}

void minSwitchTrigger(){
  switchTriggered=true;
  MinSwitchState=0; //do  even need this? If so, it will have to be switched back to 1 after switch is released
}

void maxSwitchTrigger(){
  switchTriggered=true;
  MaxSwitchState=0; //do  even need this? If so, it will have to be switched back to 1 after switch is released
}

void stepperMove(){
  /*
  if(stepperDirection==1){
    digitalWrite(dirPin, HIGH);  //contracting
  }
  else if(stepperDirection==-1){
    digitalWrite(dirPin, LOW);   //elongating
  }      // Makes 200 pulses for making one full cycle rotation
  */
      
 

  while(switchTriggered==false){
    int wait=500;  //tme delay
    force=(scale.get_units()/10000.0)*9.81;
    error=setpointForce-force;
    K_d=100;
    Serial.println(error);
    if(error<(-0.05)){
      digitalWrite(dirPin, HIGH);  
      int t=millis();
      //for(int x = 0; x < 5; x++){   // x < int(K_d*error); x++) {
      while(error<(-0.05)){
        /*
        digitalWrite(stepPin,HIGH);
        //delay(1);
        delayMicroseconds(wait);
        digitalWrite(stepPin,LOW);
        //delay(1);
        delayMicroseconds(wait);
        */

        stepper.setSpeed(-1*int(error*K_d));
        stepper.runSpeed();
        if((millis()-t)>100){
          force=(scale.get_units()/10000.0)*9.81;
          error=setpointForce-force;
          Serial.println(error);
          t=millis();
        }
      }
    }
    else if (error>(0.05)){
      //digitalWrite(dirPin, LOW);
      int t=millis();
      //for(int x = 0; x < 5; x++){   // x < int(K_d*error); x++) {
      while(error>(0.05)){
        /*
        digitalWrite(stepPin,HIGH);
        //delay(1);
        delayMicroseconds(wait);
        digitalWrite(stepPin,LOW);
        //delay(1);
        delayMicroseconds(wait);
        */

        stepper.setSpeed(-1*int(error*K_d));
        stepper.runSpeed();
        if((millis()-t)>100){
          force=(scale.get_units()/10000.0)*9.81;
          error=setpointForce-force;
          Serial.println(error);
          t=millis();
        }
      }
    }





      /*
      digitalWrite(dirPin, LOW);  
      int c=0;   
      //for(int x = 0; x < 50; x++){   //int(K_d*error); x++) {
      //while(error>(0.05)){
        /*
        digitalWrite(stepPin,HIGH); 
        //delay(1);
        delayMicroseconds(wait); 
        digitalWrite(stepPin,LOW); 
        //delay(1);
        delayMicroseconds(wait); 

        c+=1;
        if(c>40){
          force=(scale.get_units()/10000.0)*9.81;
          error=setpointForce-force;
          Serial.println(error);

          c=0;
          */
        //}
        
        
        
      //}
    //}                                              
    
    
  }
  stepperDirection*=-1;
  //delay(1000);
  switchTriggered=false;
  
}

  



