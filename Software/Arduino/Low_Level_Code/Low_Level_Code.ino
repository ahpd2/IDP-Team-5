/** TODO
 * - check if blue extra forward movement is needed.
 * - if angle is fucked on blue skip do this like the move around clockwise does.
 * - test this jankiness
 */


#ifdef VSCODE
#include <Effetors.ino>
#include <Sensors.ino>
#endif

#define DEBUG

#ifdef DEBUG
    #define debug(arg) Serial.print(arg)
    #define debugln(arg) Serial.println(arg)
#else
    #define debug(arg)
    #define debugln(arg)
#endif

void setup()
{
    Serial.begin(9600);

    setupSensors();
    setupEffectors();
}

#pragma region //Delay Setup 
void customDelay(unsigned long t)
{
    unsigned long startT = millis();
    while (millis() < startT + t) {
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
    if (isFlashing && millis() > 250 + lastTime) {
        amberLEDState = !amberLEDState;
        amberLED(amberLEDState);
        lastTime = millis();
    }
}

#define delay(arg) customDelay(arg)

#pragma endregion

bool go = false;

#define FINDINGBLOCKS       0
#define DELIVERINGBLUE      1
#define MOVINGRED           2
#define DELIVERINGRED       3
#define RETURNINGHOME       4
#define COLLECTINGMOVEDRED  5
#define DONE                6
uint8_t currentTask = FINDINGBLOCKS;

uint8_t redBlocksDelivered = 0;
bool redBlocksMoved = 0;
uint8_t blueBlocksDelivered = 0;

#define BLUE 0
#define RED 1
#define UNKNOWN -1

#define TOPLEFT 0
#define TOPRIGHT 1
#define BOTTOMLEFT 2
#define BOTTOMRIGHT 3

#define T_FROM_RIGHT    (readLine(0)    && readLine(1)                  && !readLine(3) )
#define T_FROM_LEFT     (!readLine(0)                   && readLine(2)  && readLine(3)  )
#define T_FROM_BOTTOM   (readLine(0)    && readLine(1)  && readLine(2)  && readLine(3)  )
#define BOX_OR_SPLIT    (!readLine(0)   && readLine(1)  && readLine(2)  && !readLine(3) )
#define INNER_TWO       (                  readLine(1)  && readLine(2)                  )

uint8_t lastBlockDelivered = UNKNOWN;
bool layout4 = false;

void followLine()
{   
    if (readLine(1)) {
        moveLeft(255);
    } else if (readLine(2)) {
        moveRight(255);
    } else {
        moveForward(255);
    }
    updateLED();
}

void followLineBackwards()
{
    if (readLine(1)) {
        backLeft(255);
    } else if (readLine(2)) {
        backRight(255);
    } else {
        moveBackward(255);
    }
    updateLED();
}

void dropBlock()
{
    moveStop();
    moveBackward(200);
    delay(400);
    openGrabbers();
    moveBackward(200);
    delay(1500);
}

int pickupBlock()
{
    int col = readColour();
                    
    // move forward a bit to ensure block in all the way in grabbers.
    moveForward(200);
    delay(200);
    moveStop();

    if (col == BLUE)
    {
        // move forward further for blue as it is detected earlier.
        moveForward(200);
        delay(300);
        closeGrabbers();
        blueLED(HIGH);
    } 
    else if (col == RED) {
        closeGrabbers();
        redLED(HIGH);
    }
    return col;
}

void loop()
{
    if (readGo()) // If go buttong pressed
    {
        go = !go;
        if (go) {
            isFlashing = true;
            moveForward(200);
            delay(800);
        } else {
            moveStop();
            isFlashing = false;
            amberLED(LOW);
        }
        debug(go);
        delay(200);
    }
    if (go) // Can move
    {
        updateLED();
        
        switch (currentTask){
            case FINDINGBLOCKS:
                // if layouts 3, 4 or 5 and delivered red to first delivery zone OR 1 
                if ((redBlocksMoved + blueBlocksDelivered == 1 && redBlocksDelivered != 0) // layout 3 & 5
                    || (redBlocksDelivered == 1 && blueBlocksDelivered == 2 && lastBlockDelivered == RED)
                    || (redBlocksMoved == 1 && blueBlocksDelivered == 1 && lastBlockDelivered == RED)){
                    while (readColour() == UNKNOWN)
                        followLine();
                    int col = pickupBlock();
                    // layout 3 or 4
                    if (col == BLUE){
                        // if not layout 2 last blue block
                        if ( !(redBlocksMoved == 1 && blueBlocksDelivered == 1 && lastBlockDelivered == RED)){ 
                            turnLeft();
                            moveForward(255);
                            while (!INNER_TWO)
                                delay(1);
                            if (redBlocksDelivered == 2){ // layout 5
                                turnLeft();
                            }
                            else{ // layout 3
                                turnRight(); // TODO: if angle is fucked do this like the move around clockwise does.
                            }
                        }
                        currentTask = DELIVERINGBLUE;
                    }
                    // layout 5
                    if (col == RED){
                        currentTask = DELIVERINGRED;
                    }
                }
                else {
                    // get to t junction
                    while (!BOX_OR_SPLIT)
                        followLine();
                    moveForward(255);
                    while (BOX_OR_SPLIT)
                        delay(1);
                    while (!T_FROM_BOTTOM)
                        followLine();
                    // FIRST BLOCK
                    if ((redBlocksDelivered + redBlocksMoved + blueBlocksDelivered == 0)){
                        turnLeft();
                        while (readColour() == UNKNOWN) // follow line until hit block
                            followLine();
                        
                        int col = pickupBlock();
                        turnAround();
                        if (col == BLUE)
                            currentTask = DELIVERINGBLUE; // deliver the blue block
                        else if (readColour() == RED)
                            currentTask = MOVINGRED; // move the red block to start area.
                    }
                    // SECOND BLOCK
                    if (redBlocksDelivered + redBlocksMoved + blueBlocksDelivered == 1){
                        turnRight();
                        while (readColour() == UNKNOWN) // follow line until hit block
                            followLine();
                        
                        int col = pickupBlock();
                        if (col == BLUE){
                            turnAround();
                            currentTask = DELIVERINGBLUE; // deliver the blue block
                        } 
                        else if (col == RED)
                            if (redBlocksMoved == 1)
                                layout4 = true;
                            currentTask = DELIVERINGRED; // move the red block to start area.
                    }
                    // if fourth block of layout 3 and 1 or third of 2 and 6
                    if ((redBlocksMoved == 0 && blueBlocksDelivered == 2)
                            || (redBlocksMoved == 1 && blueBlocksDelivered == 1 && lastBlockDelivered == BLUE)){
                        turnLeft();
                        while (readColour() == UNKNOWN) // follow line until hit block
                            followLine();
                        int col = pickupBlock();
                        if (col == RED){
                            currentTask == DELIVERINGRED;
                        }
                        if (col == BLUE){
                            turnAround();
                            currentTask = DELIVERINGBLUE;
                        }
                    }
                }
                break;

            case DELIVERINGBLUE:
                // go to the T junction
                while (!T_FROM_LEFT && !T_FROM_RIGHT)
                    followLine();
                if (T_FROM_RIGHT)
                    turnLeft();
                else
                    turnRight();
                // go to J junction
                while (!BOX_OR_SPLIT)
                    followLine();
                // get past J to the right
                moveRight(200);
                delay(1000);
                // go to Blue delivery area
                while (!T_FROM_BOTTOM)
                    followLine();
                // execute blue delivery
                turnLeft();
                while (!BOX_OR_SPLIT)
                    followLine();
                if (blueBlocksDelivered) {
                    dropBlock();
                    blueLED(LOW);
                    blueBlocksDelivered += 1;
                    while (!readLine(0))
                        followLineBackwards();
                    turnLeft();
                } else {
                    moveForward(200);
                    delay(300);
                    turnRight();
                    while (!BOX_OR_SPLIT)
                        followLine();
                    dropBlock();
                    blueLED(LOW);
                    blueBlocksDelivered += 1;
                    while (!BOX_OR_SPLIT)
                        followLineBackwards();
                    moveBackward(200);
                    delay(470);
                    turnRight();
                    // TODO: Go far enough to get to line before turning right again.
                    moveForward(200);
                    delay(1400);
                    while (!readLine(3))
                        followLine();
                    turnRight();
                }
                if ((redBlocksDelivered == 2 && blueBlocksDelivered == 2) // all delivered
                    || (redBlocksMoved == 1 && blueBlocksDelivered == 2)){ // or layouts 2, 4, 6
                    while (!BOX_OR_SPLIT) // go to j split
                        followLine();
                    moveForward(255);
                    while (BOX_OR_SPLIT) // get past j split
                        delay(1);
                    int t_start = millis();
                    while (millis() < t_start + 500) // move forward for another 0.5 seconds
                        followLine();
                    turnAround();
                    if (redBlocksDelivered == 2){
                        currentTask == RETURNINGHOME;
                    }
                    else{
                        currentTask == COLLECTINGMOVEDRED;
                    }
                }
                currentTask = FINDINGBLOCKS;
                lastBlockDelivered = BLUE;
                break;

            case MOVINGRED:
                // go to the T junction
                while (!T_FROM_LEFT && !T_FROM_RIGHT)
                    followLine();
                if (T_FROM_RIGHT)
                    turnLeft();
                else
                    turnRight();
                // go to J junction
                while (!BOX_OR_SPLIT)
                    followLine();
                moveForward(255);
                while (BOX_OR_SPLIT)  // move forward until past j junction         
                    delay(1);

                long t_start = millis();
                while (millis() < t_start + 2000) // follow line for two seconds
                    followLine();
                // drop block
                openGrabbers();
                redLED(LOW);
                redBlocksMoved += 1;
                t_start = millis();
                while (millis() < t_start + 1500) // follow line backwards for 1 and a half seconds
                    followLineBackwards();
                turnAround();
                currentTask = FINDINGBLOCKS;
                break;
            

            case DELIVERINGRED:
                while (!BOX_OR_SPLIT)
                    followLine();
                dropBlock();
                redLED(LOW);
                redBlocksDelivered += 1;
                
                if (redBlocksDelivered == 2 && blueBlocksDelivered == 2){
                    if (lastBlockDelivered == RED && redBlocksMoved == 0)
                        moveAroundClockwise();
                    else if (lastBlockDelivered == RED && redBlocksMoved == 1){
                        moveAroundAntiClockwise();
                    }
                    else
                        turnAround();
                    currentTask = RETURNINGHOME;
                }
                else if (blueBlocksDelivered == 2 && redBlocksMoved == 0){
                    moveAroundClockwise();
                    currentTask = FINDINGBLOCKS;
                }
                else if (blueBlocksDelivered == 2 && redBlocksMoved == 1){
                    moveAroundAntiClockwise();
                    currentTask = FINDINGBLOCKS;
                }
                else if (blueBlocksDelivered == 0 && redBlocksMoved == 1){ // layout 4 bottom right red
                    moveAroundAntiClockwise();
                    currentTask = FINDINGBLOCKS;
                }
                if (redBlocksMoved){ // layout 2 top left red
                    moveAroundClockwise();
                    currentTask = FINDINGBLOCKS;
                }
                else{
                    moveAroundAntiClockwise();
                    currentTask = FINDINGBLOCKS;
                }
                lastBlockDelivered = RED;
                break;

            case RETURNINGHOME:
                // go to the T junction
                while (!T_FROM_LEFT && !T_FROM_RIGHT && !BOX_OR_SPLIT)
                    followLine();
                if (!BOX_OR_SPLIT){
                    if (T_FROM_RIGHT)
                        turnLeft();
                    else
                        turnRight();
                    // go to J junction
                    while (!BOX_OR_SPLIT)
                        followLine();   
                }
                moveForward(255);
                while (BOX_OR_SPLIT)  // move forward until past j junction         
                    delay(1);
                while (!T_FROM_BOTTOM)
                    followLine();
                moveForward(255);
                delay(1000);
                turnAround();
                moveStop();
                go = !go;
                isFlashing = false;
                amberLED(LOW);
                currentTask = DONE;
                break;

            case COLLECTINGMOVEDRED:
                // go to split junction
                while (!BOX_OR_SPLIT)
                    followLine();   
                moveForward(255);
                while (BOX_OR_SPLIT)  // move forward until past j junction         
                    delay(1);
                while (readColour() == UNKNOWN)
                    followLine();
                // pick up red block
                int col = pickupBlock();
                if (col == RED){
                    turnAround();
                    while (!BOX_OR_SPLIT)
                        followLine();
                    moveForward(255);
                    while (BOX_OR_SPLIT)
                        delay(1);
                    while (!T_FROM_BOTTOM)
                        followLine();
                    if (layout4)
                        turnLeft();
                    else
                        turnRight();
                    currentTask == DELIVERINGRED;
                }

                break;
            default:
                debug("defaulted");
        }

    } else // if not moving, debug print line followers
    {
        /*
        debug(analogRead(A2));
        debug(", ");
        debug(analogRead(A1));
        debug(", ");
        debug(analogRead(A3));
        debug(", ");
        debugln(analogRead(A4));
        */
        //readColour();

        debug(readLine(1));
        debug(", ");
        debug(readLine(2));
        debug(", ");
        debug(readLine(0));
        debug(", ");
        debug(readLine(3));
        debug(", ");
        debugln(readColour());
        /*
        debug("distance: ");
        debugln(readDist(10));
        delay(20);
        */
    }
}


