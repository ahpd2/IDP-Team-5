
#ifdef ARDUINO and ARDUINO < 100
  #include <Sensors.ino>
  #include <Effetors.ino>
#endif
void setup()
{
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
void loop()
{
    if (readGo()) // If go buttong pressed
    { 
        go = !go;
        moveStop();
        amberLED(LOW);
        Serial.write(go);
        delay(200);
        if (go)
        {
            amberLED(HIGH);
            moveForward(200);
            delay(1000);
        }
    }
    if (go) // Can move
    {
        bool left, right, farLeft, farRight;
        left = readLine(1);
        right = readLine(2);
        farLeft = readLine(0);
        farRight = readLine(3);


        // Debug Outputs:
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


        if (readDist(3) < 5 && readDist(3) > 2) // If block infront
        {
            // TODO: Double check object infront.
            moveStop();
            if (readDist(20) < 5 && readDist(20) > 2)
            {
              if (carryingBlue)
              {
                  moveAroundAntiClockwise();
              }
              else if (carryingRed)
              {
                  moveAroundClockwise();
              }
              else
              {
                  moveAroundClockwise();
              }
            }
        }

        if (farLeft && left && !farRight) // Reaches T juction turning left.
        {
            if (red_blocks_delivered == 2 && blue_blocks_tunnel == 2) // only leave circle if all blocks delivered
            {
                turnLeft();
            }
            else
            {
                moveForward(200);
            }
        }
        else if (farRight && right && !farLeft) // Reaches T juction turning right.
        {
            // Only goes anti-clockwise when carrying a blue block, so leave into the tunnel always
            turnRight();
            if (carryingBlue)
            {
                moveForward(200); // move into tunnel section
                delay(2000);
                moveStop();
                openGrabbers(); // drop the block
                blue_blocks_tunnel += 1;
                carryingBlue = false;
                blueLED(LOW);
                moveBackward(200); // move back to avoid block
                delay(1000);
                turnAround(); // turn around
                moveForward(200);
            }
        }
        else if (left == true && right == true) // two middle see white
        {
            if (farLeft == true && farRight == true) // if coming into the T for the first time or start/finish box
            {
                if (red_blocks_delivered == 2 && blue_blocks_tunnel == 2)// if all blocks delivered, then move into start finish box and stop
                {
                  moveForward(200);
                  delay(500);
                  turnAround();
                  moveStop();
                  go = !go;
                }
                else //otherwise will be at T junction coming into oval, or at blue delivery so turn left
                {
                  turnLeft();
                }
            }
            else if (farLeft == false && farRight == false) // Reached a delivery zone of split junction
            {
                if (carryingRed) // if carrying a block, drop it ont he delivery zone
                {
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
                else // move forward otherwise (probably on splitter junction anyway)
                {
                    moveForward(200);
                }
            }
        }
        else if (left) // drifitng left from line
        {
            moveLeft(200);
        }
        else if (right) // drifting right from line
        {
            moveRight(200);
        }
        else // on line
        {
            moveForward(200);
        }

        // Colour detection:
        if (carryingBlue == false && carryingRed == false) // Dont detect colour if carrying block.
        {
            int col = readColour();
            if (col != -1) // if there is a detected colour
            {
                Serial.print(col);
                moveStop();
                if (readColour() == 0) // Blue
                {
                    closeGrabbers();
                    blueLED(HIGH);
                    carryingBlue = true;
                    turnAround();
                }
                else if (readColour() == 1) // Red
                {
                    if (red_blocks_delivered < 2)
                    {
                        closeGrabbers();
                        redLED(HIGH);
                        carryingRed = true;
                    }
                    else
                    {
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
    else // if not moving, debug print line followers
    {
        Serial.print(analogRead(A2));
        Serial.print(", ");
        Serial.print(analogRead(A1));
        Serial.print(", ");
        Serial.print(analogRead(A3));
        Serial.print(", ");
        Serial.println(analogRead(A4));
    }
}
