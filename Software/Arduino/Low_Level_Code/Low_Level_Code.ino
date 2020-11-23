
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  
  setupSensors();
  setupEffectors();
  amberLED(HIGH);
}


int go = false;
bool carryingRed = false;
bool carryingBlue = false;
int red_blocks_delivered = 0;
int blue_blocks_tunnel = 0;
int last = 0;
void loop() {
    if (readGo()){
          go = !go;
          moveStop();
          amberLED(LOW);
          Serial.write(go);
          delay(200);  
          if (go){
              amberLED(HIGH);
              moveForward(200);
              delay(1000);  
          }
      }
      if (go){     
          bool left, right, farLeft, farRight;
          left = readLine(1);
          right = readLine(2);
          farLeft = readLine(0);
          farRight = readLine(3);

          Serial.print(farLeft);
          Serial.print(", ");
          Serial.print(left);
          Serial.print(", ");
          Serial.print(right);
          Serial.print(", ");
          Serial.print(farRight);
          Serial.print(", ");
          int t = millis();
          Serial.println(t - last);
          last = t;
          if (readDist(3) < 5 && readDist(3) > 2){
            if (carryingBlue){
              moveAroundAntiClockwise();
            }
            else if (carryingRed){
              moveAroundClockwise();
            }
            else{
              moveAroundClockwise();
            }
          }
          
          
  
          if (farLeft && left && !farRight){
            if (red_blocks_delivered == 2 && blue_blocks_tunnel == 2){
              turnLeft();
            }
            else{
              moveForward(200);
              }
          }
          else if (farRight && right && !farLeft){
            turnRight();
            if (carryingBlue){
              moveForward(200);
              delay(2000);
              moveStop();
              openGrabbers();
              blue_blocks_tunnel += 1;
              carryingBlue = false;
              blueLED(LOW);
              moveBackward(200);
              delay(1000);
              turnAround();
              moveForward(200);
            } 
          }
          else if (left == true && right == true){
            if (farLeft == true && farRight == true){        
              turnLeft();
            }
            else if (farLeft == false && farRight == false){
               if (carryingRed){
                moveStop();
                moveBackward(200);
                delay(500);
                openGrabbers();
                redLED(LOW);
                red_blocks_delivered += 1;
                carryingRed = false;
                moveBackward(200);
                delay(800);
                moveAroundClockwise();
               }
               else{
                moveForward(200);
               }
             }
          }
          else if (left){
            moveLeft(200); 
          }
          else if (right){
            moveRight(200); 
          }
          else {
            moveForward(200);  
          }
          
          
          int col = readColour();
          if (carryingBlue == false && carryingRed == false){
            if (col != -1){
              Serial.print(col);
              moveStop();
              if (readColour() == 0){
                closeGrabbers();
                blueLED(HIGH);
                carryingBlue = true;
                turnAround();
              }
              else if (readColour() == 1){ 
                if (red_blocks_delivered < 2){
                  closeGrabbers();
                  redLED(HIGH);
                  carryingRed = true;
                }
                else {
                  moveBackward(200);
                  delay(500);
                  moveAroundClockwise();
                }
              }
             }
            }
          
  
          
          //move forward around line
          /*
          if (readColour() != -1){
            stopMotors();
            closeGrabbers();
            if (readColour() == 1){
              if (//before halfway){
                turnAround();
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
                    turnAround();
                    //deliver to first point
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
                      turnAround();
                      //deliver to first point
                    }
                  }
                  else{
                    if (//going anit clockwise){
                      //go forward to second delivery point
                    }
                    else{
                      turnAround(); 
                      //deliver to second point
                    }
                  }
                }
                else{
                  if (//second delivery point taken){
                    if (//going clockwise){
                      turnAround();
                      //deliver to first point
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
                      turnAround();
                      //deliver to second point
                    }
                  }
                }
              }
              red_blocks_delivered += 1;
            }
          }
        
        }
        
        }
        */
       
      }
      else{
        Serial.print(analogRead(A2));
        Serial.print(", ");
        Serial.print(analogRead(A1));
        Serial.print(", ");  
        Serial.print(analogRead(A3));
        Serial.print(", ");
        Serial.println(analogRead(A4));
        
      }
}
