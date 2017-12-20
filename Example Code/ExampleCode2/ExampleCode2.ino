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

float scale1[]={391.995,440.0,493.883,587.330,659.255,783.991,880.0,987.767,1174.66,1318.51}; 
//top half- G major pent = E minor pent G4 A4 B4 D5 E5 G5 A5 B5 D6 E6

float scale2[]={130.813,146.832,164.814, 195.998, 220.000,261.626,293.665,329.628,391.995, 440.0}; 
//bottom half- C major pent = A minor pent C3 D3 E3 G3 A3 C4 D4 E4 G4 A4

float scale3[]={146.832,164.814,184.997,220.000,246.942,293.665,329.628,369.994,440.000, 493.883};
//bottom - play D major pent = B minor pent D3 E3 F#3 A3 B3 D4 E4 F#4 A4 B4

unsigned long ts=0L; //Counter to enact delays without breaking loop  
unsigned long ts2=0L; //Counter to enact delays without breaking loop
float maxReading=0; //Ambient sensor reading for calibration
float minReading=1024; //Average sensor reading when sensor is covered
boolean autoPlay=false; //Boolean autoplay toggle

int readings[5];      // the readings from the analog input
int total =0;
int readIndex = 0; 

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

  for (int thisReading = 0; thisReading < 5; thisReading++) {
    readings[thisReading] = 0;
  }
}//setup()


void loop() {
Serial.print("left:");Serial.println(analogRead(A2));
Serial.print("right:");Serial.println(analogRead(A1));

  if(digitalRead(topButtonPin)==HIGH){
    autoPlay=true;
    analogWrite(danceLedPin,255); 
  } else{
    autoPlay=false; 
    analogWrite(danceLedPin,0); 
  }

  if((digitalRead(switchPin)==HIGH)&&(digitalRead(botButtonPin)==LOW)){ //If the play switch is on and the pause button is off
      int scaleSliderVal=analogRead(leftSliderPin);
      
      Serial.print("Scale Slider:");Serial.println(scaleSliderVal);
      Serial.print("minReading:");Serial.println(minReading);
      Serial.print("maxReading:");Serial.println(maxReading);

      if(scaleSliderVal<50) sound(scale1, -1);//top - play continuous notes
      else if(scaleSliderVal<575) sound(scale1, 10); //top half- G major pent = E minor pent GABDE
      else if (scaleSliderVal<1000) sound(scale2, 10 ); //bottom half- C major pent = A minor pent CDEGA
      else sound(scale3, 10 ); //bottom - play D major pent = B minor pent DEF#AB
  }
  else{ 
    noToneAC(); //Turn off speaker
    analogWrite(blueLedPin,LOW); 
  }//play switch is off
  
} //loop()


void sound(float thisScale[], int num_notes){
  int sensorValue = constrain(analogRead(lightSensorPin), minReading,maxReading); Serial.print("sensorValue:");Serial.println(sensorValue);
    
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = sensorValue;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= 5) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  int averageValue = total / 5;
  
  if (num_notes > 0){
    
    int closestPos=0;
    
    if (autoPlay==false) closestPos= map(sensorValue,minReading,maxReading,0,num_notes);  
    else  closestPos= map(averageValue,maxReading,maxReading*8/50+600,0,num_notes); //Autoplay, so notes optimized for autoplay
  
    int volVal=map(analogRead(rightSliderPin),0,1000,1,10); //The map HAS TO BE from 0 to 1000 or the sound quality will be bad at full volume

    toneAC(thisScale[closestPos],volVal); Serial.print("Note being played:");Serial.println(thisScale[closestPos]);
    analogWrite(blueLedPin, map(thisScale[closestPos],110,1760 ,0,255)); //Sets blue LED intensity proportional to the current note
  
  }//quantized note from one of the predefined scales
  else{
    
    int volVal=map(analogRead(rightSliderPin),0,1000,1,10); //The map HAS TO BE from 0 to 1000 or the sound quality will be bad at full volume
    double note = constrain(map(averageValue,minReading,maxReading,130, 1300), 130, 1300); 
    toneAC(note,volVal); Serial.print("Note being played:");Serial.println(note);
    analogWrite(blueLedPin, map(note,110,1760 ,0,255)); //Sets blue LED intensity proportional to the current note
  
  }//continuous, non-chromatic note
  
}//sound()
