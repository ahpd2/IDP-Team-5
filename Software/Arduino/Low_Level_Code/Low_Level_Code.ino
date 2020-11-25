//    #include <Sensors.ino>
//    #include <Effetors.ino>

void setup()
{
    Serial.begin(9600);
    // put your setup code here, to run once:

    setupSensors();
    setupEffectors();
}

void customDelay(unsigned long t)
{
    unsigned long startT = millis();
    while (millis() < startT + t)
    {
        updateLED();
    }
}

unsigned long lastTime = 0;
bool amberLEDState = false;
bool isFlashing = false;
/**
 * Run this as often as possible to update the flashing LED.
 */
void updateLED()
{
    if (isFlashing && millis() > 250 + lastTime)
    {
        amberLEDState = !amberLEDState;
        amberLED(amberLEDState);
        lastTime = millis();
    }
}

#define delay(arg) customDelay(arg)

int go = false;
bool carryingRed = false;
bool carryingBlue = false;
int red_blocks_delivered = 0;
int blue_blocks_delivered = 0;
int last = 0;
bool blueDeliverySequence = false;
bool overFirstBox = false;
int lastState = 0;
void loop()
{
    updateLED();
    if (readGo()) // If go buttong pressed
    {
        go = !go; 
        if (go)
        {
            isFlashing = true;
            moveForward(200);
            delay(800);
        }
        else {
            moveStop();
            isFlashing = false;
        }
        Serial.write(go);
        delay(200);
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

        int dist = readDist(3);
        if (carryingRed || carryingBlue){
            if (dist < 11 && dist > 3) // If block infront
            {
                // TODO: Double check object infront.
                moveStop();
                dist = readDist(20);
                if (dist < 11 && dist > 3)
                {
                  if (carryingBlue)
                  {
                      moveAroundAntiClockwise();
                  }
                  else
                  {
                      moveAroundClockwise();
                  }
                }
            }
        }
        

        if (farLeft && left && !farRight) // Reaches T juction turning left.
        {
          if (lastState == 4){
            if (red_blocks_delivered == 2 && blue_blocks_delivered) // only leave circle if all blocks delivered or if one blue block has been delivered
            {
                turnLeft();
            }
            
            else
            {
                moveForward(200);
            }
          }
          lastState = 4;
        }
        else if (farRight && right && !farLeft) // Reaches T juction turning right.
        {
          if (lastState == 3){
            // Only goes anti-clockwise when carrying a blue block, so leave into the tunnel always, at this point make blue delivery sequence = true in order to turn right at the split junction
            turnRight();
            blueDeliverySequence = true;
          }
          lastState = 3;
        }
        else if (left && right) // two middle see white
        {
            if (farLeft && farRight) // if coming into the T for the first time or start/finish box
            {
              
              if (lastState == 1){
                  if (red_blocks_delivered == 2 && blue_blocks_delivered == 2)// if all blocks delivered, then move into start finish box and stop
                  {
                    moveForward(200);
                    delay(500);
                    turnAround();
                    moveStop();
                    go = !go;
                    isFlashing = false;
                  }
                  else //otherwise will be at T junction coming into oval, or at blue delivery so turn left
                  {
                    turnLeft();
                  }
              }
              lastState = 1;
            }
            else if (!farLeft && !farRight) // Reached a delivery zone of split junction
            {
                if (lastState == 2){
                  if (carryingRed) // if carrying a block, drop it ont he delivery zone
                  {
                      moveStop();
                      moveBackward(200);
                      delay(400);
                      openGrabbers();
                      redLED(LOW);
                      red_blocks_delivered += 1;
                      carryingRed = false;
                      moveBackward(200);
                      delay(1500);
                      moveAroundClockwise();
                  }
                  else if (carryingBlue && blueDeliverySequence) // this is to turn right at the split junction, to the blue delivery zone
                  {
                    moveRight(200);
                    delay(1000);
                    blueDeliverySequence = false;
                  }
                  else if (carryingBlue && (overFirstBox || blue_blocks_delivered == 1)) // this to deliver the blue over the square
                  {
                      moveStop();
                      moveBackward(200);
                      delay(500);
                      openGrabbers();
                      blueLED(LOW);
                      blue_blocks_delivered += 1;
                      carryingBlue = false;
                      moveBackward(200);
                      delay(1500);
                      turnAround();
                      overFirstBox = false;
                  }
                  else if (carryingBlue && blue_blocks_delivered == 0) // this is to move past the first blue delivery zone if no blue blocks have been delivered yet
                  {
                      //TODO: make it move completely past the block, rather than turn right and get stuck
                      overFirstBox = true;
                      blueGoingOverDelivery();
                  }
                  else // move forward otherwise (probably on splitter junction anyway)
                  {
                      moveForward(200);
                  }
                }
                lastState = 2;
            }
        }
        else if (left) // drifitng left from line
        {
            lastState = 0;
            moveLeft(200);
        }
        else if (right) // drifting right from line
        {
          lastState = 0;
            moveRight(200);
        }
        else // on line
        {
          lastState = 0;
            moveForward(200);
        }

        // Colour detection:
        if (carryingBlue == false && carryingRed == false) // Dont detect colour if carrying block.
        {
            int col = readColour();
            if (col != -1) // if there is a detected colour
            {
                moveForward(200);
                delay(200); 
                moveStop();
                if (readColour() == 0) // Blue
                {
                    moveForward(200);
                    delay(300);
                    closeGrabbers();
                    blueLED(HIGH);
                    carryingBlue = true;
                    rotateLeft(255);
                    delay(1600);
                    moveForward(200);
                    delay(1000);
                    openGrabbers();
                    blueLED(LOW);
                    carryingBlue = false;
                    blue_blocks_delivered += 1;
                    moveBackward(200);
                    delay(1000);
                    rotateRight(255);
                    delay(1600);
                }
                else if (readColour() == 1) // Red
                {
                    if (red_blocks_delivered < 2)
                    {
                      closeGrabbers();
                      redLED(HIGH);
                      carryingRed = true;
                      if (blue_blocks_delivered < 2){
                        turnAround();
                        openGrabbers();
                        redLED(LOW);
                        carryingRed = false;
                        moveBackward(200);
                        delay(1300);
                        turnAround();
                      }
                    }
                    else
                    {
                        moveBackward(200);
                        delay(1500);
                        moveAroundClockwise();
                    }
                }
            }
        }

        //move forward around line

    }
    else // if not moving, debug print line followers
    {
        /*
        Serial.print(analogRead(A2));
        Serial.print(", ");
        Serial.print(analogRead(A1));
        Serial.print(", ");
        Serial.print(analogRead(A3));
        Serial.print(", ");
        Serial.println(analogRead(A4));
        */
        
        Serial.print(readLine(1));
        Serial.print(", ");
        Serial.print(readLine(2));
        Serial.print(", ");
        Serial.print(readLine(0));
        Serial.print(", ");
        Serial.print(readLine(3));
        Serial.print(", ");
        Serial.println(readColour());
        /*
       Serial.print("distance: ");
       Serial.println(readDist(10));
       delay(20);
       */
    }
}
