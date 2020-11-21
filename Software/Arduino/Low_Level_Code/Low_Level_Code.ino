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

        
        
      }
      else{
        int x = readDist(5);
        if (x > 0){
          Serial.println();
        }
        delay(10);
      }
}
