
// Stop/go button
#define GO_BUTTON 0

// Line Sensors

#define LINE_LEFT A2
#define LINE_MID_LEFT A1
#define LINE_MID_RIGHT A3
#define LINE_RIGHT A4

#define LEFT_THRESH 800
#define MID_LEFT_THRESH 950
#define MID_RIGHT_THRESH 930
#define RIGHT_THRESH 980

// Colour Sensor
#define COL_RED 3
#define COL_BLUE 4
#define COL_LDR A0
#define COL_DELAY 5 //in ms
#define COL_THRESH 20 //min difference between colours to define a detection

// Distance Sensor
#define DIST_TRIG 8
#define DIST_ECHO 11

// Photointerruptors
#define INT_1 2
#define INT_2 1




void setupSensors(){
    pinMode(GO_BUTTON, INPUT_PULLUP);

    pinMode(LINE_LEFT, INPUT);
    pinMode(LINE_MID_RIGHT, INPUT);  
    pinMode(LINE_MID_LEFT, INPUT);
    pinMode(LINE_RIGHT, INPUT);  

    pinMode(COL_RED, OUTPUT);
    pinMode(COL_BLUE, OUTPUT);
    pinMode(COL_LDR, INPUT);

    pinMode(DIST_TRIG, OUTPUT);
    pinMode(DIST_ECHO, INPUT);

    pinMode(INT_1, INPUT);
    pinMode(INT_2, INPUT);
}

void testSensors(){
    Serial.println("==========Testing Sensors=========");

    Serial.print("Go button: ");
    Serial.println(digitalRead(GO_BUTTON));

    Serial.print("Line Left: ");
    Serial.println(analogRead(LINE_LEFT));
    Serial.print("Line Mid Left: ");
    Serial.println(analogRead(LINE_MID_LEFT));
    Serial.print("Line Mid Right: ");
    Serial.println(analogRead(LINE_MID_RIGHT));
    Serial.print("Line RIGHT: ");
    Serial.println(analogRead(LINE_RIGHT));

    Serial.print("Colour Sensor: ");
    int col = readColour();
    if (col == 1){
        Serial.println("Red");
    } else if (col == 0){
        Serial.println("Blue");
    } 
    else {
        Serial.println("Neither colour");
    }

    Serial.print("Photointerrupter 1: ");
    Serial.println(digitalRead(INT_1));
    Serial.print("Photointerrupter 2: ");
    Serial.println(digitalRead(INT_2));
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
  //Serial.print(red);
  //Serial.print(", ");
  //Serial.println(blue);
  if (red > blue + COL_THRESH){ // RED
    return 0;  
  }
  if (blue > red + COL_THRESH){ // BLUE
    return 1;  
  }
  return -1; // Neither
}

void readLine()

/*
  Serial.print(analogRead(LINE_LEFT));
  Serial.print(", ");
  Serial.print(analogRead(LINE_MID_LEFT));
  Serial.print(", ");
  Serial.print(analogRead(LINE_MID_RIGHT));
  Serial.print(", ");
  Serial.println(analogRead(LINE_RIGHT));
*/
