/**
 * @file    mBot_Amazeing_Race_Codes_Group_3A4.ino
 * @author  Group 3A-4 Ng Andre, Mun Le Zong, Ngo Phuc Cuong, Muhammad Ashraf Bin Mohamad Jaffar
 * @date    13/11/2020
 * @brief   Description: This file contains the codes for our mBot to run through a maze
 */
#include "MeMCore.h"

MeUltrasonicSensor ultraSensor(PORT_3);
MeLineFollower lineFinder(PORT_2);
MeLightSensor lightSensor(PORT_8);
MeRGBLed led(0, 30);
MeDCMotor motorL(M2); //left
MeDCMotor motorR(M1); //Right

//Definitions
#define SPEEDRIGHT 200
#define SPEEDLEFT 210
#define LEFTTURN 315
#define RIGHTTURN 305
#define UTURN 630
#define ULTRATURN 12
#define ULTRAWALL 3
#define RIGHTIR 600
#define LEFTIR 480
#define RIGHTPORT A0
#define LEFTPORT A1
#define BLACK_R 458
#define BLACK_G 317
#define BLACK_B 346
#define WHITE_R 745
#define WHITE_G 544
#define WHITE_B 583
//Musical Note Definitions
#define NOTE_B4  494
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_FS5 740
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_B5  988


//Initialisation
long rightir = 0;
long leftir = 0;
MeBuzzer buzzer;

//Function plays celebratory tune when color challenge returns black 
void victory_tune(){
  int melody[] = {
    NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, NOTE_B4, NOTE_E5, 
    NOTE_E5, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5, 
    NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, NOTE_D5, NOTE_FS5, 
    NOTE_FS5, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5
  };

  //TAKE ON MEEE
  int noteDurations[] = {
    8, 8, 8, 4, 4, 4, 
    4, 5, 8, 8, 8, 8, 
    8, 8, 8, 4, 4, 4, 
    4, 5, 8, 8, 8, 8
  };
  int songLength = sizeof(melody)/sizeof(melody[0]);
  for (int thisNote = 0; thisNote < songLength; thisNote++) {
    // to calculate the note duration, take one second divided by the note type.    
    int noteDuration = 1000/noteDurations[thisNote];
    buzzer.tone(8, melody[thisNote],noteDuration);
    // to distinguish the notes, set a minimum time between them.
    int pauseBetweenNotes = noteDuration * 0.5;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    buzzer.noTone(8);
  }
}

//Function detects whether black strip has been encountered on the floor
bool blackLine(){
  int sensorState = lineFinder.readSensors();
  if (sensorState == S1_IN_S2_IN || sensorState == S1_OUT_S2_IN || sensorState == S1_IN_S2_OUT){
    return true;
  }
  return false;
}

//Function checks for the colour of paper presented when it detects a black strip on floor
void colorChal(){
  //Flash the 3 RGB colours and collect the LDR value for each colour to determine overall colour
  int red = redlight();
  int green = greenlight();
  int blue = bluelight();
  if ((red >= 130 && red <= 190) && (green >= 50 && green <= 90) && (blue >= 30 && blue <= 90)){
    //RED indicates left turn
    leftTurn(); 
  } else if ((red >= 50 && red <= 110) && (green >= 110 && green <= 170) && (blue >= 70 && blue <= 130)){
    //GREEN indicates right turn
    rightTurn(); 
  } else if ((red >= 230 && red <= 280) && (green >= 190 && green <= 235) && (blue >= 120 && blue <= 165)){
    //YELLOW indicates u turn
    uTurn(); 
  } else if ((red >= 110 && red <= 150) && (green >= 120 && green <= 160) && (blue >= 150 && blue <= 200)){
    //PURPLE indicates 2 left turns, after first left turn, we use the ultrasonic sensor to determine
    //when to do another left turn
    leftTurn(); 
    motorL.stop();
    motorR.stop();
    delay(100);    
    motorL.run(-SPEEDLEFT);
    motorR.run(SPEEDRIGHT);
    for (int i = 0; i < 50; i++) {
      delay(100);
      ultraSensor.distanceCm(); //read from ultrasensor
      if (ultraSensor.distanceCm() < ULTRATURN) {
        motorL.stop();
        motorR.stop();
        delay(100);
        leftTurn();
        motorL.stop();
        motorR.stop();
        delay(100);
        break;
      }
    }
  } else if ((red >= 110 && red <= 190) && (green >= 160 && green <= 250) && (blue >= 160 && blue <= 250)) {
    //BLUE indicates 2 right turns, after first right turn, we use the ultrasonic sensor to determine
    //when to do another right turn
    rightTurn();
    motorL.stop();
    motorR.stop();
    delay(100);
    motorL.run(-SPEEDLEFT);
    motorR.run(SPEEDRIGHT);
    for (int i = 0; i < 50; i++) {
      delay(100);
      ultraSensor.distanceCm();
      if (ultraSensor.distanceCm() < ULTRATURN){
        motorL.stop();
        motorR.stop();
        delay(100);
        rightTurn();
        motorL.stop();
        motorR.stop();
        delay(100);     
        break;
      }
    }
  } else if ((red >= -10 && red <= 20) && (green >= -10 && green <= 20) && (blue >= -10 && blue <= 20)){
    //BLACK indicates maze has ended and mBot will play celebratory music
    motorL.stop(); 
    motorR.stop();
    victory_tune(); 
  }
}

//Flashes the RED LED light and collect 50 LDR readings and returns the average value
long redlight(){
  long redval = 0;
  //Set LED to RED
  led.setColor(255,0,0);
  led.show();
  delay(100);
  for (int i = 0; i < 50; i++){
    redval += lightSensor.read();
  }
  offlight();
  redval /= 50; //Take the average value of 50 readings
  //Convert reading  to within the range calibrated with BLACK and WHITE readings
  redval = (redval - BLACK_R)*255/(WHITE_R - BLACK_R);
  return redval;
}

//Flashes the GREEN LED light and collect 50 LDR readings and returns the average value
long greenlight(){
  long greenval = 0;
  //Set LED to GREEN
  led.setColor(0,255,0);
  led.show();
  delay(100);
  for (int i = 0; i < 50; i++){
    greenval += lightSensor.read();
  }
  offlight();  
  greenval /= 50; //Take the average value of 50 readings
  //Convert reading  to within the range calibrated with BLACK and WHITE readings
  greenval = (greenval - BLACK_G)*255/(WHITE_G - BLACK_G); 
  return greenval;
}

//Flashes the BLUE LED light and collect 50 LDR readings and returns the average value
long bluelight(){
  long blueval = 0;
  //Set LED to BLUE
  led.setColor(0,0,255);
  led.show();
  delay(100);
  for (int i = 0; i < 50; i++){
    blueval += lightSensor.read();
  }
  offlight();
  blueval /= 50; //Take the average value of 50 readings
  //Convert reading  to within the range calibrated with BLACK and WHITE readings
  blueval = (blueval - BLACK_B)*255/(WHITE_B - BLACK_B);
  return blueval;
}

//Off the light by setting RGB to (0,0,0)
void offlight(){
  led.setColor(0,0,0);
  led.show();  
}

//Make the mBot turn left
void leftTurn(){
  motorL.run(SPEEDLEFT);
  motorR.run(SPEEDRIGHT);
  delay(LEFTTURN);
  motorL.stop();
  motorR.stop();
}

//Make the mBot turn right
void rightTurn(){
  motorL.run(-SPEEDLEFT);
  motorR.run(-SPEEDRIGHT);
  delay(RIGHTTURN);
  motorL.stop();
  motorR.stop();
}

//Make the mBot uturn in the same grid
void uTurn(){
  motorL.run(SPEEDLEFT);
  motorR.run(SPEEDRIGHT);
  delay(UTURN);
  motorL.stop();
  motorR.stop();
}

//Setup mBot, initialise RGB LED, Right IR and Left IR
void setup() {
  Serial.begin(9600);
  led.setpin(13);
  pinMode(RIGHTPORT, INPUT);
  pinMode(LEFTPORT, INPUT);
}

//Infinite loop for mBot to move forward
void loop() {
    Serial.println(ultraSensor.distanceCm());
    
    //Main motor driver  
    motorL.run(-SPEEDLEFT);
    motorR.run(SPEEDRIGHT);
    
    //Take right and left IR readings
    rightir = analogRead(RIGHTPORT);
    Serial.print("Right:");
    Serial.print(rightir);
    leftir = analogRead(LEFTPORT);
    Serial.print(" Left:");
    Serial.println(leftir);
    
    /**
     * Condition if black line detected below mBot
     * call for colour challenge
     */
    if (blackLine()){
      motorL.stop();
      motorR.stop();
      colorChal();
    }
    
    /**
     * Condition if mBot swerves to the right wall,
     * if right IR value goes below the threshold (600),
     * left motor will stop to straighten the mBot's path
     */
    if (rightir < RIGHTIR){
      motorL.stop();
      motorR.run(SPEEDRIGHT);
      delay(100);
      motorL.run(-SPEEDLEFT);
      motorR.run(SPEEDRIGHT);
    }
    
    /**
     * Condition if mBot swerves to the left wall,
     * if left IR value goes below the threshold (500),
     * right motor will stop to straighten the mBot's path
     */
    if (leftir < LEFTIR){
      motorL.run(-SPEEDLEFT);
      motorR.stop();
      delay(100);
      motorL.run(-SPEEDLEFT);
      motorR.run(SPEEDRIGHT);
    }

    /**
     * Condition if mBot is too close to the wall infront,
     * mBot will reverse
     */
    if (ultraSensor.distanceCm() < ULTRAWALL){
      motorL.stop();
      motorR.stop();
      delay(100);
      motorL.run(SPEEDLEFT);
      motorR.run(-SPEEDRIGHT);
      delay(500);
      motorL.stop();
      motorR.stop();
    }
}
