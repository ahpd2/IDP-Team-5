
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  
  setupSensors();
  setupEffectors();
  
}


int go = false;
void loop() {
    if (readGo()){
          go = !go;
          delay(200);
          Serial.write(go);
          moveStop();
          
      }
      if (go){
        bool left, right, farLeft, farRight;
        bool carryingRed = false;
        bool carryingBlue = false;
        int red_blocks_delivered = 0;
        int blue_blocks_tunnel = 0;
        left = readLine(1);
        right = readLine(2);
        farLeft = readLine(0);
        farRight = readLine(3);
        
        if (readDist < 12){
          if (carryingBlue){
            moveAroundAntiClockwise();
          }
          else if (carryingRed){
            moveAroundClockwise();
          }
        }

        if (farLeft && left && !farRight){
          if (red_blocks_delivered == 2 && blue_blocks_tunnel == 2){
            turnLeft();
          }
          else{
            moveForward(200);
            delay(1000);
          }
        }
        if (farRight && right && !farLeft){
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
        if (left == true && right == true){
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
              delay(500);
              moveAroundClockwise();
             }
             else{
              moveForward(200);
              delay(400);
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
      */

        
        
      }
      else{
        int x = readDist(5);
        if (x > 0){
          Serial.println();
        }
        delay(10);
      }
}
