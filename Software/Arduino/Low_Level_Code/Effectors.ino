#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#ifdef VSCODE
    #include <Sensors.ino>
#endif

// Ouptut LEDS
#define LED_AMBER 7
#define LED_BLUE 6
#define LED_RED 5

// Servos
#define GRABBER_LEFT 10
#define GRABBER_RIGHT 9

#define delay(arg) customDelay(arg)

// define the servos for the grabber
Servo grabberLeft;
Servo grabberRight;

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *motorLeft = AFMS.getMotor(2);
Adafruit_DCMotor *motorRight = AFMS.getMotor(1);

void setupEffectors()
{
    grabberLeft.attach(GRABBER_LEFT);
    grabberRight.attach(GRABBER_RIGHT);
    AFMS.begin();
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
    openGrabbers();

    pinMode(LED_AMBER, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
}

void testEffectors()
{
    Serial.println("======TESTING EFFECTORS=======");
    Serial.println("Testing Servos");
    closeGrabbers();
    delay(1000);
    openGrabbers();
    delay(500);
    Serial.println("Testing Motors");
    moveForward(50);
    delay(1000);
    moveLeft(50);
    delay(1000);
    moveRight(50);
    delay(1000);
    moveBackward(50);
    delay(1000);
    Serial.println("TEsting High Speed");
    moveForward(255);
    delay(200);
    moveBackward(255);
    delay(200);
    moveStop();

    Serial.println("Testing LEDS");
    digitalWrite(LED_AMBER, HIGH);
    Serial.println("Amber LED");
    delay(500);
    digitalWrite(LED_AMBER, LOW);
    digitalWrite(LED_BLUE, HIGH);
    Serial.println("Blue LED");
    delay(500);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_RED, HIGH);
    Serial.println("Red  LED");
    delay(500);
    digitalWrite(LED_RED, LOW);
}

void amberLED(bool val)
{
    digitalWrite(LED_AMBER, val);
}

void blueLED(bool val)
{
    digitalWrite(LED_BLUE, val);
}

void redLED(bool val)
{
    digitalWrite(LED_RED, val);
}

void closeGrabbers()
{
    grabberLeft.write(84);
    grabberRight.write(58);
}

void openGrabbers()
{
    grabberLeft.write(48);
    grabberRight.write(89);
}

void moveForward(int vel)
{
    motorLeft->setSpeed(vel);
    motorRight->setSpeed(vel);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
}

void moveLeft(int vel)
{
    motorLeft->setSpeed(vel);
    motorRight->setSpeed(vel);
    motorLeft->run(RELEASE);
    motorRight->run(FORWARD);
}

void rotateLeft(int vel)
{
    motorLeft->setSpeed(vel);
    motorRight->setSpeed(vel);
    motorLeft->run(BACKWARD);
    motorRight->run(FORWARD);
}

void moveRight(int vel)
{
    motorLeft->setSpeed(vel);
    motorRight->setSpeed(vel);
    motorLeft->run(FORWARD);
    motorRight->run(RELEASE);
}

void backRight(int vel)
{
    motorLeft->setSpeed(vel);
    motorRight->setSpeed(vel);
    motorLeft->run(RELEASE);
    motorRight->run(BACKWARD);
}

void backLeft(int vel)
{
    motorLeft->setSpeed(vel);
    motorRight->setSpeed(vel);
    motorLeft->run(BACKWARD);
    motorRight->run(RELEASE);
}

void rotateRight(int vel)
{
    motorLeft->setSpeed(vel);
    motorRight->setSpeed(vel);
    motorLeft->run(FORWARD);
    motorRight->run(BACKWARD);
}

void moveBackward(int vel)
{
    motorLeft->setSpeed(vel);
    motorRight->setSpeed(vel);
    motorLeft->run(BACKWARD);
    motorRight->run(BACKWARD);
}

void moveStop()
{
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
}

void turnLeft()
{
    moveForward(200);
    delay(400);
    rotateLeft(255);
    delay(1600);
    moveStop();
}

void turnRight()
{
    moveForward(200);
    delay(400);
    rotateRight(255);
    delay(1600);
    moveStop();
}

void turnAround()
{
    rotateRight(255);
    delay(3200);
    moveStop();
}

void moveAroundAntiClockwise()
{
    turnLeft();
    moveForward(200);
    delay(300);
    // Move in an arc until it hits the line
    motorLeft->setSpeed(200);
    motorRight->setSpeed(65);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    delay(300);
    for (int x = 0; x < 1150; x++)
    { // If no line is hit after 10 seconds stop anyway
        if (readLine(2)) // Hit line
            break;
        delay(10);
    }
    moveForward(200);
    delay(400);
    for (int x = 0; x < 1150; x++)
    { // If no line is hit after 11.5 seconds stop anyway
        rotateLeft(255);
        if (readLine(2)) // Hit line
            break;
        delay(10);
    }
}

void moveAroundClockwise()
{
    turnRight();
    moveForward(200);
    delay(300);
    // Move in an arc until it hits the line
    motorLeft->setSpeed(65);
    motorRight->setSpeed(200);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    delay(300);
    for (int x = 0; x < 1150; x++)
    { // If no line is hit after 10 seconds stop anyway
        if (readLine(1)) // Hit line
            break;
        delay(10);
    }
    moveForward(200);
    delay(400);
    for (int x = 0; x < 1150; x++)
    { // If no line is hit after 11.5 seconds stop anyway
        rotateRight(255);
        if (readLine(1)) // Hit line
            break;
        delay(10);
    }
    
}

void blueGoingOverDelivery()
{
  motorRight->setSpeed(40);
  motorLeft->setSpeed(255);
  motorLeft->run(FORWARD);
  motorRight->run(BACKWARD);
  delay(2100);
}
