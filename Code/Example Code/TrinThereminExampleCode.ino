/*
Example Code for the TrinTheremin

Written by Charles Cunningham in 2017
In assosiation with the Trinity School Computer Science department
Written for the TrinTheremin project, initial version (v1)

Visit for more info and resources for the TrinTheremin project:
www.trintherem.in

*/

//==================================================================
//==================================================================

//Pin Definitions---------------------------------------------------
//digital inputs
#define switchPin 12 //The switch is on digital pin 12
#define topButtonPin 4 //The top button is on digital pin 4
#define botButtonPin 8 //The bottom button is on digital pin 8
//analog inputs
#define leftSliderPin 2 //The left slider is on analog pin 2
#define rightSliderPin 1 //The right slider is on analog pin 1
#define lightSensorPin 0 //The light sensor is on analog pin 0
//digital or analog outputs
#define greenLedPin 6 //The green (left) led is on digital pin 6
#define blueLedPin 5 //The blue (right) led is on digital pin 5
#define danceLedPin 11 //The dancing led is on digital pin 11
/*When you define something in Arduino it replaces all references
to that constant with the defined value during code compilation
(under the hood).   Thus the defined value does not take up 
space on the Arduino as a variable or constant*/

//Libraries---------------------------------------------------------
#include <toneAC.h> //The speaker can be controlled with toneAC
//Library reference: https://playground.arduino.cc/Code/ToneAC

//Variables---------------------------------------------------------
//Scales
float scale0[] = {261.63,293.66,329.63,349.23,392.00,440.00,
493.88,523.25}; //Major C Scale
byte scaleLength0=8; //Scale 0 Length

float scale1[]={440,495,556.875,660,742.5}; //Pentatonic Scale
byte scaleLength1=5; //Scale 1 Length

float scale2[]={9.1770,11.5623,13.7500,18.3540,23.1247,30.8677,
36.7081,46.2493,61.7354,73.4162,97.9989,123.4708,146.8324,
195.9977,246.9417,329.6276,391.9954,493.8833,
659.2551,783.9909}; //Pentatonic D Scale
byte scaleLength2=20; //Scale 2 Length

float scale3[]={196.00,220.00,246.94,261.63,293.66,
329.63,369.99,392.00,440.00,493.88,523.25,587.33,659.25,
739.99,783.99};//G Scale
byte scaleLength3=15; //Scale 3 Length

//Other Variables
unsigned long ts=0; //Counter to enact delays without breaking loop
unsigned long ts2=0; //Counter to enact delays without breaking loop
unsigned long avgReading=0; //Average sensor reading for calibration
unsigned long counter=0; //Iteration counter for calibration
boolean autoPlay=false; //Boolean autoplay toggle



//==============================SETUP===============================
void setup() {
  //inputs
  pinMode(switchPin,INPUT); //Set the switch as an input
  pinMode(topButtonPin,INPUT); //Set the bot button as an input
  pinMode(botButtonPin,INPUT); //Set the top button as an input
  //outputs
  pinMode(danceLedPin,OUTPUT); //Set the dancing LED as an output
  pinMode(blueLedPin,OUTPUT); //Set the blue LED as an output
  pinMode(greenLedPin,OUTPUT); //Set the green LED as an output

  
  //Calibration-------------------------------vvv------------------
  ts=millis(); //Start counter at current clock time
  ts2=millis(); //Start counter for alternating LEDs
  while(millis()-ts<5000){ //While clock time - start time is < 5s
    counter++; //Running counter of how many readings are taken
    avgReading+=analogRead(0); //Add current reading to avgReading

    //Alternating blue/green LEDs to show calibration in progress
    if(millis()-ts2>100){ //For the first 100 milliseconds:
      analogWrite(blueLedPin,10); //Blue light on
      analogWrite(greenLedPin,0); //Green light off
    }
    if(millis()-ts2>200){ //For the second 100 milliseconds:
      analogWrite(blueLedPin,0); //Blue LED off
      analogWrite(greenLedPin,20); //Green LED on
      ts2=millis();
    }
  //End of Calibration Light Display
    
  }
  avgReading/=counter;//set avgReading to the average sensor reading
  //End of Calibration--------------------------^^^-----------------


  //Startup Light Sequence----------------------vvv-----------------
  digitalWrite(greenLedPin,LOW);  //All lights off
  digitalWrite(blueLedPin,LOW);   
  delay(700);                     //Delay for 700ms
  digitalWrite(greenLedPin,HIGH); //Green light on
  delay(500);                     //Delay for 500ms
  digitalWrite(blueLedPin,HIGH);  //Blue light on
  delay(250);                     //Delay for 250ms
  digitalWrite(greenLedPin,LOW);
  digitalWrite(blueLedPin,LOW);   //All lights off
  delay(250);                     //Delay for 250ms
  digitalWrite(greenLedPin,HIGH); 
  digitalWrite(blueLedPin,HIGH);  //All lights on
  delay(500);                     //Delay for 500ms
  //End Startup Light Sequence--------------------^^^----------------
  }



//==============================LOOP=================================
void loop() {

  
  //Check if the autoplay button is pressed--------vvv---------------
  if(digitalRead(topButtonPin)==HIGH){
    autoPlay=true; //Enable autoPlay
    analogWrite(danceLedPin,255); //Turn on dancing LED
  } else{
    autoPlay=false; //Disable autoPlay
    analogWrite(danceLedPin,0); //Turn off dancing LED
  }
  //-------------------------------^^^------------------------------


  //Set current scale (depending on right slider)----------vvv------
  if(digitalRead(switchPin)==HIGH){ //If the play switch is on
    if(digitalRead(botButtonPin)==LOW){ //If the pause button is off
      int scaleSliderVal=analogRead(rightSliderPin);
      if(scaleSliderVal<250){
        sound(scale1,scaleLength1);
      }
      else{ //a
        if(scaleSliderVal<500){
          sound(scale2,scaleLength2); //Call the sound function
        }
        else{ //b
          if(scaleSliderVal<750){
            sound(scale0,scaleLength0); //Call the sound function
          }
          else{ //c
            sound(scale3,scaleLength3); //Call the sound function
          } //c
        } //b
      } //a
    }
  }
  else{ //If play switch is off
    noToneAC(); //Turn off speaker
    analogWrite(blueLedPin,0); //Turn off blue LED
  }
  //--------------------------------^^^-----------------------------
  
} //End of loop





//===========================SOUND FUNCTION=========================
void sound(float thisScale[],int thisLength){
   
   int minVal=0; //Usual Minimum Light Sensor Reading (0-1023)
   int maxVal=0; //Usual Maximum Light Sensor Reading (0-1023)
   int closestPos=0; //The note position in the scale array (TBD)
   double closestVal=10000000000;//vvv
   //^^^closest note value to light sensor reading (TBD)
   //^^^This is big so its initial value will never be the closest


  //Pick note-------------------vvv---------------------------------
  if(autoPlay==false){ //If autoplay is off
    minVal=200; //Usual value when sensor is fully covered
    maxVal=avgReading; //Reading cannot exceed ambient light level
    for(int i=0;i<thisLength;i++){
      float thisVal=thisScale[i]; //Save current position to thisVal
      if(abs(map(analogRead(lightSensorPin),minVal,maxVal,thisScale[0],thisScale[thisLength-1])-thisVal)<closestVal){
        closestVal=abs(map(analogRead(lightSensorPin),200,avgReading,thisScale[0],thisScale[thisLength-1])-thisVal);
        closestPos=i;
      }
    }
  }
  else{ //If autoplay is on (notes optimized for autoplay)
    minVal=600; //A range of 600-800 works best with the dancing LED
    maxVal=800; //Ambient values not considered (dancing LED on)
    for(int i=0;i<thisLength;i++){
      float thisVal=thisScale[i];
      if(abs(map(analogRead(lightSensorPin),minVal,maxVal,thisScale[0],thisScale[thisLength-1])-thisVal)<closestVal){
        closestVal=abs(map(analogRead(lightSensorPin),600,800,thisScale[0],thisScale[thisLength-1])-thisVal);
        closestPos=i;
      }
    }
  }
  //Note is Picked---------------^^^--------------------------------

  int volVal=map(analogRead(leftSliderPin),0,1023,0,9.5);
  //^^^ Sets volume (0-9.5 is a usable range)
  
  toneAC(thisScale[closestPos],volVal);
  //toneAC(note,volume (0-10)); 
  //^^^ See reference link at top for full toneAC syntax
  
  analogWrite(blueLedPin,map(thisScale[closestPos],thisScale[0],thisScale[thisLength-1],0,255)); //overflow
  //^^^ Sets blue LED intensity proportional to the current note

}
//==================================================================
