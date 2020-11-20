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
        
        if (left && right && farLeft && farRight){
          Serial.println("Skeet");
          Serial.println("My Meat");
          moveStop();
          amberLED(HIGH);
          delay(3000);
          moveLeft(255);
          delay(1000);  
        }
        else if (farLeft && farRight == false){
          if (right){
            moveLeft(200);
          }
          else{
            moveForward(200);
            delay(1000);
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
        
      }
      else{
        Serial.println("Readings:");
        Serial.println(readLine(0));
        Serial.println(readLine(1));
        Serial.println(readLine(2));
        Serial.println(readLine(3));
      }
}
