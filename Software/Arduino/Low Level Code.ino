#include <Servo.h>


Servo grabber1; 
Servo grabber2;

#define GO_BUTTON 13
#define LINE_LEFT A0
#define LINE_RIGHT A1

#define COL_RED 0
#define COL_BLUE 1
#define COL_LDR A2
#define COL_DELAY 5
#define COL_THRESH 20


#define THRESHOLD 950

#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *motorLeft = AFMS.getMotor(1);
Adafruit_DCMotor *motorRight = AFMS.getMotor(2);

void setup() {

  Serial.begin(9600);
  // put your setup code here, to run once:
  grabber1.attach(10);
  grabber2.attach(9);

  pinMode(LINE_LEFT, INPUT);
  pinMode(LINE_RIGHT, INPUT);  
  pinMode(GO_BUTTON, INPUT_PULLUP);

  pinMode(COL_RED, OUTPUT);
  pinMode(COL_BLUE, OUTPUT);
  pinMode(COL_LDR, INPUT);

  AFMS.begin();
  openGrabbers();
}

int go = false;

void loop() {
  if (!digitalRead(GO_BUTTON)){
      go = !go;
      delay(200);
      Serial.write(go);
      motorLeft->run(RELEASE);
      motorRight->run(RELEASE);
  }
  if (go){
    if (analogRead(LINE_LEFT) > THRESHOLD){
      motorLeft->setSpeed(150);
      motorLeft->run(BACKWARD); 
      Serial.print("Left ");
    }
    else{
      motorLeft->run(RELEASE);
    }
    if (analogRead(LINE_RIGHT) > THRESHOLD){
      motorRight->setSpeed(150);
      motorRight->run(BACKWARD );
      Serial.print("Right "); 
    }
    else{
      motorRight->run(RELEASE);
    }

    int col = readColour();
    if (col != -1){
      Serial.print(col);
      motorLeft->run(RELEASE);
      motorRight->run(RELEASE);
      closeGrabbers();
      go = false;
    }
    
    Serial.println("");
  }
}


void closeGrabbers(){
  grabber1.write(90);             
  grabber2.write(60);          
}

void openGrabbers(){
  grabber1.write(60);             
  grabber2.write(90);          
}



int readColour(){
  digitalWrite(COL_RED, HIGH);
  delay(COL_DELAY);
  int red = analogRead(COL_LDR);
  digitalWrite(COL_RED, LOW);
  digitalWrite(COL_BLUE, HIGH);
  delay(COL_DELAY);
  int blue = analogRead(COL_LDR);
  digitalWrite(COL_BLUE, LOW);
  if (red < blue - COL_THRESH){
    return 0;  
  }
  if (blue < red - COL_THRESH){
    return 1;  
  }
  return -1;
}
