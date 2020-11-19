#define LINE_LEFT A2
#define LINE_MID_LEFT A1
#define LINE_MID_RIGHT A3
#define LINE_RIGHT A4

#define THRESHOLD 950

#define GO_BUTTON 0

void setup() {

  Serial.begin(9600);
  // put your setup code here, to run once:
  
  setupSensors();
  setupEffectors();
  
}


int go = false;
void loop() {
    if (!digitalRead(GO_BUTTON)){
          go = !go;
          delay(200);
          Serial.write(go);
          moveStop();
          
      }
      if (go){
        int left, right, farLeft, farRight;
        left = analogRead(LINE_MID_LEFT);
        right = analogRead(LINE_MID_RIGHT);
        farLeft = analogRead(LINE_LEFT);
        farRight = analogRead(LINE_RIGHT);
        
        if (left < THRESHOLD && right < THRESHOLD){
          if (farLeft < THRESHOLD && farRight < THRESHOLD)  {
            moveLeft(100);  
          }
          else{
            moveForward(100);
          }
        }
        else if (left < THRESHOLD){
          moveLeft(100); 
          Serial.print("Left ");
        }
        else if (right < THRESHOLD){
          moveRight(100); 
          Serial.print("Right "); 
        }
        else {
          moveForward(100);  
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
     else {
       Serial.print(analogRead(LINE_LEFT));
       Serial.print(", ");
       Serial.print(analogRead(LINE_MID_LEFT));
       Serial.print(", ");
       Serial.print(analogRead(LINE_MID_RIGHT));
       Serial.print(", ");
       Serial.println(analogRead(LINE_RIGHT)); 
     }
}
