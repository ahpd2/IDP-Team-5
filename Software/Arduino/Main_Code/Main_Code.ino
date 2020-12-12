/** TODO
 * - check if blue extra forward movement is needed.
 * - if angle is fucked on blue skip do this like the move around clockwise does.
 * - second blue delivery reliability
 * - left red retrieval
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

#define FINDINGBLOCKS       0
#define DELIVERINGBLUE      1
#define MOVINGRED           2
#define DELIVERINGRED       3
#define RETURNINGHOME       4
#define COLLECTINGMOVEDRED  5
#define DONE                6
uint8_t currentTask = FINDINGBLOCKS;

uint8_t redBlocksDelivered = 0;
uint8_t redBlocksMoved = 0;
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
bool go = false;

#define SPEED 200
#define HIGHSPEED 255

// SETUP FUNCTION:
void setup()
{
    Serial.begin(9600);

    setupSensors();
    setupEffectors();
}

void followLine()
{   
    if (readLine(1)) {
        moveLeft(SPEED);
    } else if (readLine(2)) {
        moveRight(SPEED);
    } else {
        moveForward(HIGHSPEED);
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
            //redBlocksMoved = 1;
            debugln("start");
        } else {
            moveStop();
            isFlashing = false;
            amberLED(LOW);
        }
        delay(200);
    }
    if (!go) // Can't move yet
    {
        // if not moving, debug print line followers
        
        debug(readLine(1)); debug(", ");
        debug(readLine(2)); debug(", ");
        debug(readLine(0)); debug(", ");
        debug(readLine(3)); debug(", ");
        debugln(readColour());
        /*
        debug(analogRead(A2)); debug(", ");
        debug(analogRead(A1)); debug(", ");
        debug(analogRead(A3)); debug(", ");
        debugln(analogRead(A4));
         */ 
    }
    else // can move
    {   
        debugln(currentTask);
        int currentTaskCopy = currentTask;
        switch (currentTaskCopy){
            case FINDINGBLOCKS: {
                if (lastBlockDelivered == RED){
                    while (readColour() == UNKNOWN)
                        followLine();
                    int col = pickupBlock();
                    // layout 3 or 4
                    if (col == BLUE){
                        // if not layout 2 last blue block
                        if ( !(redBlocksMoved == 1 && blueBlocksDelivered == 1 && lastBlockDelivered == RED)){ 
                            turnLeft(); // cut across to save time
                            moveForward(HIGHSPEED);
                            while (!INNER_TWO)
                                delay(1);
                            if (redBlocksDelivered == 2){ // layout 5
                                turnLeft();
                            }
                            else{ // layout 3
                                turnRight(); // TODO: if angle is fucked do this like the move around clockwise does.
                            }
                        }
                        else {
                          turnRight(); // cut across to save time
                          moveForward(HIGHSPEED);
                          while (!INNER_TWO)
                              delay(1);
                          turnRight();
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
                    if (lastBlockDelivered == BLUE){
                        moveLeft(SPEED);
                    }
                    else{
                        moveForward(SPEED);
                    }         
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
                            debugln("Exiting finding blocks");
                        } 
                        else if (col == RED){
                            if (redBlocksMoved == 1)
                                layout4 = true;
                            currentTask = DELIVERINGRED; // move the red block to start area.
                        }
                    }
                    // if fourth block of layout 3 and 1 or third of 2 and 6
                    else if ((redBlocksMoved == 0 && blueBlocksDelivered == 2) || (redBlocksMoved == 1 && blueBlocksDelivered == 1 && lastBlockDelivered == BLUE)){
                        debugln("Yeet");
                        turnLeft();
                        while (readColour() == UNKNOWN){ // follow line until hit block
                            followLine();
                        }
                        int col = pickupBlock();
                        if (col == RED){
                            currentTask = DELIVERINGRED;
                            debug("task set to delivering red: ");
                            debugln(currentTask);
                        }
                        else if (col == BLUE){
                            turnAround();
                            currentTask = DELIVERINGBLUE;
                        }
                    }
                }
                debug("Task at end of findingblocks: ");
                debugln(currentTask);
                break;
            }

            case DELIVERINGBLUE: {
                debugln("Delivering Blue");
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
                // keep turning right until mid left sensor sees the line
                while(!readLine(1)){
                  delay(1);
                }
                // go to Blue delivery area
                while (!T_FROM_BOTTOM)
                    followLine();
                // execute blue delivery
                turnLeft();
                while (!BOX_OR_SPLIT)
                    followLine();
                if (blueBlocksDelivered) {
                    debugln("delivering blue 2");
                    moveBackward(200);
                    delay(100);
                    dropBlock();
                    blueLED(LOW);
                    blueBlocksDelivered += 1;
                    debugln("dropped");
                    turnLeft();
                    if (redBlocksDelivered == 2 || redBlocksMoved){ // either done or theres a red stashed
                        debugln("Yeet");
                        moveRight(SPEED);
                        delay(700);
                        moveForward(HIGHSPEED);
                        delay(4000);
                        if (redBlocksDelivered == 2){
                            currentTask = RETURNINGHOME;
                        }
                        else{
                            currentTask = COLLECTINGMOVEDRED;
                        }
                        lastBlockDelivered = BLUE;
                        break;                       
                    }
                    else{
                      moveForward(SPEED);
                      delay(1000);
                    }
                } else {
                    moveForward(200);
                    delay(200);
                    turnRight();
                    moveForward(SPEED);
                    delay(600);
                    while (!BOX_OR_SPLIT)
                        followLine();
                        
                    dropBlock();
                    blueLED(LOW);
                    blueBlocksDelivered += 1; // delivered
                    while (!BOX_OR_SPLIT)
                        moveBackward(200);
                    moveBackward(200);
                    delay(470);
                    turnRight();
                    // TODO: Go far enough to get to line before turning right again.
                    moveForward(200);
                    delay(1600);
                    while (!readLine(3))
                        followLine();
                    turnRight();
                }
                
                currentTask = FINDINGBLOCKS;
                lastBlockDelivered = BLUE;
                break;
            }

            case MOVINGRED: {
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
                moveForward(SPEED);
                while (BOX_OR_SPLIT)  // move forward until past j junction         
                    delay(1);

                long t_start = millis();
                while (millis() < t_start + 11000) // follow line for two seconds
                    followLine();
                // drop block
                openGrabbers();
                redLED(LOW);
                redBlocksMoved += 1;
                t_start = millis();
                moveBackward(SPEED);
                delay(2000);
                turnAround();
                currentTask = FINDINGBLOCKS;
                break;
            }

            case DELIVERINGRED: {
                debugln("delivering red");
                while (!BOX_OR_SPLIT)
                    followLine();
                dropBlock();
                redLED(LOW);
                redBlocksDelivered += 1;
                
                if (redBlocksDelivered == 2 && blueBlocksDelivered == 2){
                    if (lastBlockDelivered == RED && redBlocksMoved == 0){
                        moveAroundClockwise();
                    }
                    else if (lastBlockDelivered == RED && redBlocksMoved == 1){
                        moveAroundAntiClockwise();
                    }
                    else{
                        turnAround();
                    }
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
                    moveForward(SPEED);
                    while (BOX_OR_SPLIT)  // move forward until past j junction         
                        delay(1);
                    currentTask = RETURNINGHOME;
                    break;
                }
                else if ((blueBlocksDelivered == 0 || blueBlocksDelivered == 2) && redBlocksMoved == 1){ // layout 4 bottom right red
                    moveAroundAntiClockwise();
                }
                else if (redBlocksMoved || (blueBlocksDelivered == 2 && redBlocksMoved == 0)){ // layout 2 top left red
                    moveAroundClockwise();
                }
                else{
                    moveAroundAntiClockwise();
                }
                currentTask = FINDINGBLOCKS;
                lastBlockDelivered = RED;
                break;
            }

            case RETURNINGHOME: {
                while (!T_FROM_BOTTOM)
                    followLine();
                moveForward(SPEED);
                delay(1000);
                turnAround();
                moveStop();
                go = !go;
                isFlashing = false;
                amberLED(LOW);
                currentTask = DONE;
                break;
            }

            case COLLECTINGMOVEDRED: {
                // go to block
                while (readColour() == UNKNOWN){
                   followLine();
                }
                // pick up red block
                int col = pickupBlock();
                if (col == RED){
                    debugln("Picking up stored red");
                }
                else{
                    debugln("BLOCK WRONG COLOUR!");
                }
                turnAround();
                while (!BOX_OR_SPLIT)
                    followLine();
                moveForward(SPEED);
                while (BOX_OR_SPLIT)
                    delay(1);
                while (!T_FROM_BOTTOM)
                    followLine();
                if (layout4)
                    turnLeft();
                else
                    turnRight();
                currentTask = DELIVERINGRED;
                break;
            }

            default: {
                debugln("defaulted");
                break;
            }
        }
    } 
}
