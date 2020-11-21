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
        left = readLine(1);
        right = readLine(2);
        farLeft = readLine(0);
        farRight = readLine(3);
        
        if (farLeft && left && !farRight){
          blueLED(HIGH);
          turnLeft();
          blueLED(LOW);
        }
        if (left == true && right == true){
          if (farLeft == true && farRight == true){
            amberLED(HIGH);            
            turnLeft();
            amberLED(LOW);
          }
          else if (farLeft == false && farRight == false){
             moveForward(200);
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
        
        /*
        int col = readColour();
        if (col != -1){
          Serial.print(col);
          motorLeft->run(RELEASE);
          motorRight->run(RELEASE);
          closeGrabbers();
          go = false;
        }
        */
        /*if (readDist < ){
          moveBackward(200);
          delay(700);
          turnLeft();
          moveForward(200);
          delay(2000);
          turnRight();
          moveForward(200);
          delay(3000);
          turnRight();
          moveForward(200);
          delay(2000);
          turnLeft();
          delay(500);  
        }
        */

        
        //move forward around line
        
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
      else{
        int x = readDist(5);
        if (x > 0){
          Serial.println();
        }
        delay(10);
      }
}
//zowwo
//Testing123
