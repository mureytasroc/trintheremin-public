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

float scale1[]={440,495,556.875,660,742.5}; //Pentatonic Scale

float scale2[]={61.7354,73.4162,97.9989,123.4708,146.8324,
195.9977,246.9417,329.6276,391.9954,493.8833,
659.2551,783.9909}; //Pentatonic D Scale

float scale3[]={196.00,220.00,246.94,261.63,293.66,
329.63,369.99,392.00,440.00,493.88,523.25,587.33,659.25,
739.99,783.99};//G Scale

//Other Variables
unsigned long ts=0L; //Counter to enact delays without breaking loop
unsigned long ts2=0L; //Counter to enact delays without breaking loop
float avgReading=0; //Ambient sensor reading for calibration
float lowAvg=0; //Average sensor reading when sensor is covered
float counter=0; //Iteration counter for calibration
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
  //Ambient Reading
  ts=millis(); //Start counter at current clock time
  ts2=millis(); //Start counter for alternating LEDs
  while(millis()-ts<3500){ //While clock time - start time is < 3.5s
    counter++; //Running counter of how many readings are taken
    avgReading+=analogRead(0); //Add current reading to avgReading

    //Blue LED blinking to show ambient calibration in progress
    if(millis()-ts2>400){ //For the first 400 milliseconds:
      digitalWrite(blueLedPin,HIGH); //Blue light on
    }
    if(millis()-ts2>800){ //For the second 400 milliseconds:
      digitalWrite(blueLedPin,LOW); //Blue LED off
      ts2=millis();
    }
  //End of Calibration Light Display

  
  }
  avgReading/=counter;//set avgReading to the average sensor reading

  digitalWrite(blueLedPin,LOW);
  digitalWrite(greenLedPin,LOW); //Set both LEDs to LOW to tell the user to put their hand over the sensor
  toneAC(500,4); //Play sound to tell user to put their hand over the sensor
  delay(1500); //Gives the user time to put their hand on the sensor 
  noToneAC();
  
  //Low Reading
  ts=millis(); //Start counter at current clock time
  ts2=millis(); //Start counter for alternating LEDs
  counter=0; //Reset counter
  while(millis()-ts<3500){ //While clock time - start time is < 3.5s
    counter++; //Running counter of how many readings are taken
    lowAvg+=analogRead(0); //Add current reading to lowAvg
    
    //Both LEDs blinking to show low level calibration in progress
    if(millis()-ts2>400){ //For the first 400 milliseconds:
      digitalWrite(blueLedPin,HIGH); //Blue light on
      digitalWrite(greenLedPin,HIGH); //Green light on
    }
    if(millis()-ts2>800){ //For the second 400 milliseconds:
      digitalWrite(blueLedPin,LOW); //Blue LED off
      digitalWrite(greenLedPin,LOW); //Green LED off
      ts2=millis();
    }
  //End of Calibration Light Display

  }
  lowAvg/=counter;//set avgLowReading to the average sensor reading
  //End of Calibration--------------------------^^^-----------------


  //Startup Light Sequence----------------------vvv-----------------
  digitalWrite(greenLedPin,LOW);  //All lights off
  digitalWrite(blueLedPin,LOW);   
  delay(200);                     //Delay for 200ms
  digitalWrite(greenLedPin,HIGH); //Green light on
  delay(100);                     //Delay for 100ms
  digitalWrite(blueLedPin,HIGH);  //Blue light on
  delay(100);                     //Delay for 100ms
  digitalWrite(greenLedPin,LOW);
  digitalWrite(blueLedPin,LOW);   //All lights off
  delay(100);                     //Delay for 100ms
  digitalWrite(greenLedPin,HIGH); 
  digitalWrite(blueLedPin,HIGH);  //All lights on
  delay(200);                     //Delay for 200ms
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
  if((digitalRead(switchPin)==HIGH)&&(digitalRead(botButtonPin)==LOW)){ //If the play switch is on and the pause button is off
      int scaleSliderVal=analogRead(rightSliderPin);
      if(scaleSliderVal<250){
        sound(scale1);
      }
      else{ //a
        if(scaleSliderVal<500){
          sound(scale2); //Call the sound function
        }
        else{ //b
          if(scaleSliderVal<750){
            sound(scale0); //Call the sound function
          }
          else{ //c
            sound(scale3); //Call the sound function
          } //c
        } //b
      } //a
    
  }
  else{ //If play switch is off
    noToneAC(); //Turn off speaker
    analogWrite(blueLedPin,0); //Turn off blue LED
  }
  //--------------------------------^^^-----------------------------
  
} //End of loop





//===========================SOUND FUNCTION=========================
void sound(float thisScale[]){
  int thisLength=(int)(sizeof(thisScale)); //Number of notes in scale
  float minVal=0; //Minimum light sensor value (determined below)
  float maxVal=0; //Maximum light sensor value (determined below)
  int closestPos=0; //The note position in the scale array (TBD)
  double closestVal=10000000000;//vvv
   //^^^closest note value to light sensor reading (TBD)
   //^^^This is big so its initial value will never be the closest


  //Pick note-------------------vvv---------------------------------
  if(autoPlay==false){ //If autoplay is off
    minVal=lowAvg; //Usual value when sensor is fully covered
    maxVal=avgReading; //Reading cannot exceed ambient light level
    for(int i=0;i<thisLength;i++){
      float thisVal=thisScale[i]; //Save current position to thisVal
      if(abs(map(analogRead(lightSensorPin),minVal,maxVal,thisScale[0],thisScale[thisLength-1])-thisVal)<closestVal){
        closestVal=abs(map(analogRead(lightSensorPin),minVal,maxVal,thisScale[0],thisScale[thisLength-1])-thisVal);
        closestPos=i;
      }
    }
  }
  else{ //If autoplay is on (notes optimized for autoplay)
    minVal=lowAvg+500; //Dancing LED provides its own light so constants are added to the ambient light levels.
    maxVal=avgReading+300; //These numbers were found to work best with the dancing LED.
    for(int i=0;i<thisLength;i++){
      float thisVal=thisScale[i];
      if(abs(map(analogRead(lightSensorPin),minVal,maxVal,thisScale[0],thisScale[thisLength-1])-thisVal)<closestVal){
        closestVal=abs(map(analogRead(lightSensorPin),minVal,maxVal,thisScale[0],thisScale[thisLength-1])-thisVal);
        closestPos=i;
      }
    }
  }
  //Note is Picked---------------^^^--------------------------------

  int volVal=map(analogRead(leftSliderPin),0,1000,0,10); //The map HAS TO BE from 0 to 1000 or the sound quality will be bad at full volume
  //^^^ Sets volume (0-9.5 is a usable range)
  
  toneAC(thisScale[closestPos],volVal);
  //toneAC(note,volume (0-10)); 
  //^^^ See reference link at top for full toneAC syntax
  
  analogWrite(blueLedPin,map(thisScale[closestPos],thisScale[0],thisScale[thisLength-1],0,255)); //overflow
  //^^^ Sets blue LED intensity proportional to the current note
}
//==================================================================
