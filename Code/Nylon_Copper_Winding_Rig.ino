/*
 * William Donaldson 2018
 * Code for a rig that winds nichrome wire around nylon fishing lines
 */


/* Uncomment following if you want to use the 1.8" LCD
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define TFT_CS     6
#define TFT_SCLK   5   
#define TFT_MOSI   4 
#define TFT_DC     3  
#define TFT_RST    2 
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
*/

#include <AccelStepper.h>
AccelStepper rightStepper(1, 25, 24); 
AccelStepper leftStepper(1, 27, 26); 
AccelStepper beltStepper(1, 29, 28); 

int togglePin=50;
int Direction=1;
bool Move=false;
int limitSwitch=21; //not currently being used, add in later if you want
int potPin=A0;      //not currently being used, add in later if you want

int MaxSpd=800;
int spd=80;        //ratio between these two speed variables will determine the nichrome coil density
int spinSpd=800;   //the following equality must be maintained: spinSpd<=MaxSpd

void setup() {
  pinMode(togglePin, INPUT);
  rightStepper.setMaxSpeed(MaxSpd);
  leftStepper.setMaxSpeed(MaxSpd);
  beltStepper.setMaxSpeed(MaxSpd);

  /* Following is not in use, but may be useful in the future
  pinMode(limitSwitch, INPUT);  
  attachInterrupt(digitalPinToInterrupt(limitSwitch), trigger, CHANGE);
  
  pinMode(potPin, INPUT);

  tft.setRotation(3);
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);
  delay(1000);
  */

  
}

void loop() {  
   if(digitalRead(togglePin)==LOW){
    Serial.println(Direction);
    Direction*=-1;
    if (Move == true){
      Move=false;
    }
    else if (Move == false){
      Move=true;
      spd*=Direction;
    }
    delay(300);
   }
   
   if(Move==true){
    spinFilament(spinSpd);
    beltStepper.setSpeed(spd);
    beltStepper.runSpeed();
   }
   else if(Move==false){
    spinFilament(0);
    beltStepper.setSpeed(0);
    beltStepper.runSpeed();
   }
}

void spinFilament(int spd){
  //spins a filament strung between two stepper motors 

  rightStepper.setSpeed(spd);
  leftStepper.setSpeed(-spd); 
  rightStepper.runSpeed();
  leftStepper.runSpeed();
}

/*Not currently being used, but may provide insight if you choose to program the LCD
void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}
*/
