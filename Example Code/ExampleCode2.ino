/*
Example Code 2 for the TrinTheremin
Written by Charles Cunningham and J. Gohde in 2017
In association with the Trinity School Computer Science department
Written for the TrinTheremin project, initial version (v1)
Visit for more info and resources for the TrinTheremin project:
www.trintherem.in
*/

//==================================================================
//==================================================================
//Libraries---------------------------------------------------------
#include <toneAC.h> //The speaker can be controlled with toneAC
//Reference: https://playground.arduino.cc/Code/ToneAC

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

unsigned long ts=0L; //Counter to enact delays without breaking loop
unsigned long ts2=0L; //Counter to enact delays without breaking loop
float maxReading=0; //Ambient sensor reading for calibration
float minReading=1024; //Average sensor reading when sensor is covered
boolean autoPlay=false; //Boolean autoplay toggle



//==============================SETUP===============================
void setup() {
  //configure inputs
  pinMode(switchPin,INPUT); //Set the switch as an input
  pinMode(topButtonPin,INPUT); //Set the bot button as an input
  pinMode(botButtonPin,INPUT); //Set the top button as an input
  //configure outputs
  pinMode(danceLedPin,OUTPUT); //Set the dancing LED as an output
  pinMode(blueLedPin,OUTPUT); //Set the blue LED as an output
  pinMode(greenLedPin,OUTPUT); //Set the green LED as an output

  
  //Calibration-------------------------------vvv------------------

  ts=millis(); //Start counter at current clock time
  while(millis()-ts<5000){ //While clock time - start time is < 5s
    int reading=analogRead(0); //Add current reading to avgReading
    if(reading > maxReading) maxReading = reading;
    else if (reading < minReading) minReading = reading;
    
    //Blue LED blinking to show ambient calibration in progress
    if(millis()%800 ==0){ //For the first 400 milliseconds:
      digitalWrite(blueLedPin,HIGH); //Blue light on
    }
    else if(millis()%400==0){ //For the second 400 milliseconds:
      digitalWrite(blueLedPin,LOW); //Blue LED off
    }
  }  
  
  digitalWrite(blueLedPin,LOW);
  digitalWrite(greenLedPin,LOW); //Set both LEDs to LOW to tell the user to put their hand over the sensor
 
  //Begin Startup Light Sequence----------------------vvv-----------------
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
 
  Serial.begin(9600);
  Serial.print("minReading:");
  Serial.print(minReading);
  Serial.print("maxReading:");
  Serial.print(maxReading);
}//setup()


void loop() {

  if(digitalRead(topButtonPin)==HIGH){
    autoPlay=true;
    analogWrite(danceLedPin,255); 
  } else{
    autoPlay=false; 
    analogWrite(danceLedPin,0); 
  }

  if((digitalRead(switchPin)==HIGH)&&(digitalRead(botButtonPin)==LOW)){ //If the play switch is on and the pause button is off
      int scaleSliderVal=analogRead(rightSliderPin);
      
      Serial.print("Scale Slider:");Serial.println(scaleSliderVal);
      Serial.print("minReading:");Serial.println(minReading);
      Serial.print("maxReading:");Serial.println(maxReading);

      if(scaleSliderVal<250) sound(scale0, 8);
      else if(scaleSliderVal<500) sound(scale1, 5); 
      else if(scaleSliderVal<750) sound(scale2, 12 ); 
      else sound(scale3, 15 ); 
  }
  else{ 
    noToneAC(); //Turn off speaker
    analogWrite(blueLedPin,LOW); 
  }//play switch is off
  
} //loop()



void sound(float thisScale[], int num_notes){
  int scaleLength= num_notes; //Number of notes in scale
  Serial.print("scaleLength:");Serial.println(scaleLength);
  Serial.print(sizeof(thisScale));Serial.println(sizeof(float));

  int closestPos=0;

  int sensorValue = analogRead(lightSensorPin);
  Serial.print("sensorValue:");Serial.println(sensorValue);
  
  if (sensorValue < minReading) sensorValue = minReading;
  else if (sensorValue > maxReading) sensorValue = maxReading;
  
  Serial.print("sensorValue:");Serial.println(sensorValue);

  if(autoPlay==false) closestPos=map(sensorValue,minReading,maxReading,0,scaleLength);  
  else  closestPos=map(sensorValue,minReading+500,maxReading+300,0,scaleLength); //Autoplay, so notes optimized for autoplay
  
  Serial.print("closestPos:");Serial.println(closestPos);

  int volVal=map(analogRead(leftSliderPin),0,1000,0,10); //The map HAS TO BE from 0 to 1000 or the sound quality will be bad at full volume
  //^^^ Sets volume (0-10 is a usable range)
  
  Serial.print("Vol (0-10):");Serial.println(volVal);
  
  toneAC(thisScale[closestPos],volVal);
  Serial.print("Note being played:");Serial.println(thisScale[closestPos]);
  
  analogWrite(blueLedPin, map(thisScale[closestPos],thisScale[0],thisScale[scaleLength-1],0,255)); //Sets blue LED intensity proportional to the current note

}//sound()
