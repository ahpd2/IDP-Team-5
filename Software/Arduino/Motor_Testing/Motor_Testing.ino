#include <Servo.h>
#include "SparkFunLSM6DS3.h"
#include "Wire.h"
#include "SPI.h"


Servo grabber1; 
Servo grabber2;

#define GO_BUTTON 13
#define LINE_LEFT A2
#define LINE_RIGHT A1

#define COL_RED 0
#define COL_BLUE 1
#define COL_LDR A0
#define COL_DELAY 5
#define COL_THRESH 20

#define DIST_THRESH 40

#define LED_AMBER 7
#define LED_BLUE 6
#define LED_RED 5


#define THRESHOLD 950

#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor_RIGHT = AFMS.getMotor(2);
Adafruit_DCMotor *myMotor_LEFT = AFMS.getMotor(1);
// You can also make another motor on port M2
//Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);

LSM6DS3 builtin_IMU(SPI_MODE, SPIIMU_SS);  // SPI Chip Select

struct coord {
  int x;
  int y;
};

coord velocity = {0, 0};
coord previous_position = {0, 0};
coord current_position = {0, 0};
coord new_position = {0, 0};
coord acceleration = {0, 0};
unsigned long time;

void setup() {

  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000); //relax...
  Serial.println("Left, Right");
  
  //Call .begin() to configure the IMU
  builtin_IMU.begin();

  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  grabber1.attach(10);
  grabber2.attach(9);

  pinMode(LINE_LEFT, INPUT);
  pinMode(LINE_RIGHT, INPUT);  
  pinMode(GO_BUTTON, INPUT_PULLUP);

  pinMode(COL_RED, OUTPUT);
  pinMode(COL_BLUE, OUTPUT);
  pinMode(COL_LDR, INPUT);

  pinMode(LED_AMBER, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  
  myMotor_LEFT->run(BACKWARD);
  myMotor_RIGHT->run(BACKWARD);

  

  AFMS.begin();
  openGrabbers();
}

int go = false;
int lines_crossed = 0;
int old_rightmotor_speed = 0;
int old_leftmotor_speed = 0;

void loop() {
  //Serial.println(encoderPosition();
  Serial.println(encoderAngle());
  delay(100);

  //digitalWrite(LED_AMBER, HIGH);
  //digitalWrite(LED_RED, HIGH);
  //digitalWrite(LED_BLUE, HIGH);

  //Serial.print(analogRead(LINE_LEFT));
  //Serial.print(", ");
  //Serial.println(analogRead(LINE_RIGHT));
  

/*
 if (!digitalRead(GO_BUTTON)){
      go = !go;
      delay(200);
      Serial.write(go);
      myMotor_LEFT->run(RELEASE);
      myMotor_RIGHT->run(RELEASE);
  }
  if (go){
    if (analogRead(LINE_LEFT) > THRESHOLD){
      myMotor_LEFT->setSpeed(150);
      myMotor_LEFT->run(BACKWARD); 
      Serial.print("Left ");
    }
    else{
      myMotor_LEFT->run(RELEASE);
    }
    if (analogRead(LINE_RIGHT) > THRESHOLD){
      myMotor_RIGHT->setSpeed(150);
      myMotor_RIGHT->run(BACKWARD );
      Serial.print("Right "); 
    }
    else{
      myMotor_RIGHT->run(RELEASE);
    }

    int col = readColour();
    if (col != -1){
      Serial.print(col);
      myMotor_LEFT->run(RELEASE);
      myMotor_RIGHT->run(RELEASE);
      closeGrabbers();
      go = false;
    }
    
    Serial.println("");
    
  }
  */
}

void straight(){
  // GO STRAIGHT
  myMotor_LEFT->setSpeed(200);
  myMotor_RIGHT->setSpeed(200);
  myMotor_RIGHT->run(BACKWARD);
  myMotor_LEFT->run(BACKWARD); 
  }
  
void turnLeft(){
  // TURN LEFT
  myMotor_LEFT->setSpeed(200);
  myMotor_RIGHT->setSpeed(200);
  myMotor_RIGHT->run(FORWARD);
  myMotor_LEFT->run(BACKWARD);
   }
   
void turnRight(){
  // TURN RIGHT
  myMotor_LEFT->setSpeed(200);
  myMotor_RIGHT->setSpeed(200);
  myMotor_RIGHT->run(BACKWARD);
  myMotor_LEFT->run(FORWARD);
}

void stopMotors(){
  // STOP THE MOTORS
  myMotor_LEFT->setSpeed(0);
  myMotor_RIGHT->setSpeed(0);
  myMotor_RIGHT->run(RELEASE);
  myMotor_LEFT->run(RELEASE);
}

void closeGrabbers(){
  // CLOSE THE GRABBERS
  grabber1.write(90);             
  grabber2.write(60);          
}

void openGrabbers(){
  // OPEN THE GRABBERS
  grabber1.write(60);             
  grabber2.write(90);          
}

int readColour(){
  // READ COLOUR SENSOR 
  digitalWrite(COL_RED, HIGH);
  delay(COL_DELAY);
  int red = analogRead(COL_LDR);
  digitalWrite(COL_RED, LOW);
  digitalWrite(COL_BLUE, HIGH);
  delay(COL_DELAY);
  int blue = analogRead(COL_LDR);
  digitalWrite(COL_BLUE, LOW);
  // IF COLOUR SENSOR DETECTS OBJECT RETURN 1 OR 0
  if (red < blue - COL_THRESH){
    return 0;  
  }
  if (blue < red - COL_THRESH){
    return 1;  
  }
  return -1;
}

void readEncoder(){
  // READ ACCELEROMETER IN X Y Z DIRECTION
  Serial.print("\nAccelerometer:\n");
  Serial.print(" X = ");
  Serial.println(builtin_IMU.readFloatAccelX(), 4);
  Serial.print(" Y = ");
  Serial.println(builtin_IMU.readFloatAccelY(), 4);
  Serial.print(" Z = ");
  Serial.println(builtin_IMU.readFloatAccelZ(), 4);

  // READ GYROSCOPE IN X Y Z DIRECTION
  Serial.print("\nGyroscope:\n");
  Serial.print(" X = ");
  Serial.println(builtin_IMU.readFloatGyroX(), 4);
  Serial.print(" Y = ");
  Serial.println(builtin_IMU.readFloatGyroY(), 4);
  Serial.print(" Z = ");
  Serial.println(builtin_IMU.readFloatGyroZ(), 4);
  delay(1000);
}

void leftMotor(int newspeed) {
  // TO REDUCE TRAFFIC SO THAT SPEED OF MOTOR IS ONLY UPDATED WHEN THERE IS NEW VELOCITY 
  if (newspeed==old_leftmotor_speed)
     return;
  else {
     // send a command that sets the left motor speed to the new speed, then ...
     old_leftmotor_speed=newspeed;
  }
}

void rightMotor(int newspeed) {
  // TO REDUCE TRAFFIC SO THAT SPEED OF MOTOR IS ONLY UPDATED WHEN THERE IS NEW VELOCITY 
  if (newspeed==old_rightmotor_speed)
     return;
  else {
     // send a command that sets the right motor speed to the new speed, then ...
     old_rightmotor_speed=newspeed;
  }
}


int old_time = 0;
int new_time = 0;
int dt = 0;


void encoderPosition(){

  old_time = new_time;
  new_time = millis();

  dt = (new_time - old_time) / 1000;
  

  coord acceleration = {builtin_IMU.readFloatAccelX(), builtin_IMU.readFloatAccelY()};
    
  new_position.x = 2 * current_position.x - previous_position.x + dt* dt * acceleration.x;

  new_position.y = 2 * current_position.y - previous_position.y + dt* dt * acceleration.y;


  previous_position = current_position;
  current_position = new_position;

  return current_position;
}

int angular_velocity = 0;
int new_angle = 0;
int current_angle = 0;

int encoderAngle(){

  old_time = new_time;
  new_time = millis();

  dt = (new_time - old_time) / 1000;

  if (abs(builtin_IMU.readFloatGyroZ()) < 4){
    angular_velocity = 0; 
  }
  else{
    angular_velocity = builtin_IMU.readFloatGyroZ();
  }
  
  new_angle = current_angle + angular_velocity * dt;

  current_angle = new_angle;

  return current_angle;
}

void gettingPastTunnel(){


}

int red_blocks_delivered = 0;
int blue_blocks_delivered = 0;

void redDeliveryZone(){
/*
  //move forward around line
  
  if (readColour() != -1){
    stopMotors();
    closeGrabbers();
    if (readColour() == 1){
      if (//before halfway){
        //do 180
        //go back to junction
      }
      else{
        //go forward to junction
      }
    blue_blocks_delivered += 1;
    }
    if (readColour() == 0){
      if (//before halfway){
        if (//FIRST delivery taken){
          //take shortest path to second point following line
        }
        else{
          if (//going clockwise){
            //go forward and deliver to first point
          }
          else{
            //do 180, deliver to first point
          }
        }
      }    
      else{
        if (//after second delivery point){
          if (//second delivery point taken){
            if (//going anti clockwise){
              //go forward, deliver to first point
            }
            else{
              //do 180, deliver to first point
            }
          }
          else{
            if (//going anit clockwise){
              //go forward to second delivery point
            }
            else{
              //do 180, deliver to second point
            }
          }
        }
        else{
          if (//second delivery point taken){
            if (//going clockwise){
              //do 180, deliver to first point
            }
            else{
              //deliver to first point
            }
          }
          else{
            if (//going clockwise){
              //deliver to second point
            }
            else{
              //do 180, deliver to second point
            }
          }
        }
      }
      red_blocks_delivered += 1;
    }
  }

}
*/
void blueDelivery(){

  
}

void followLine(){
  
}

void failureDetection(){
  //CHECK SENSOR DATA
  if (){
    
    //IMU
    if (isDigit(builtin_IMU.readFloatAccelX()) && isDigit(builtin_IMU.readFloatAccelY()) && isDigit(builtin_IMU.readFloatGyroZ())){
      return;
    }
    else{
      //Use photo-interrupter for straight line velocity measurement take compare frames of camera to measure direction
    }
    
    //COLOUR SENSORS
    if (isDigit(analogRead(COL_LDR))){
      return;
    }
    else{
      //Critical component. STOP and fix
    }
    
    //DISTANCE SENSORS
    if (isDigit(analogRead(DIST_SENSER))){
      return;
    }
    else{
      //Use camera vision to detect objects
    }
    
    //LINE FOLLOWERS
    if (isDigit(analogRead(LINE_LEFT))) && isDigit(analogRead(LINE_RIGHT))){
      return;
    }
    if (//1 or 2 of outer ones stop working){
      //use inner ones and computer vision for the junctions
    }
    else{
      //Critical component. STOP and fix
    }
    
    //PHOTOINTERRUPTERS
    if (isDigit(analogRead(PHOTO_LEFT)) && isDigit(analogRead(PHOTO_RIGHT)){
      return;
    }
    else{
      //Use IMU for velocity and camera for direction
    }
    
    //CAMERA
    /*if (//arduino is connected to computer){
      return;
    }
    else{
      //Use Photointerrupters for direction, IMU + Photointerrupters for velocity, distance sensors for object detection
    }*/
  }
  
  // CHECK ITS STILL ON LINE
  
  // CHECK IT DOESNT DETECT OBSTACLE
  if (analogRead(DIST_SENSER) < DIST_THRESH){
    if(readcolour() != -1){
      return;
    }
    else{
      /*if (// following line){
        //ignore obstacle detection
      }
      else{
        lostRobot();
      }
      */
    }
  }
  else{
    return;
  }
  
  // COMPARE READINGS FROM DIFFERENT  SENSORS 
}

void goAroundBlock(){
  
}

void turn180(){
  
}

void deliveryAction(){
  
}

void lostRobot(){
  //use IMU to retrace steps
  //move 1.5cm at a time

  
}
