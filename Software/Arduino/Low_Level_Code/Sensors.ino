
// Stop/go button
#define GO_BUTTON 0

// Line Sensors

#define LINE_LEFT A2
#define LINE_MID_LEFT A1
#define LINE_MID_RIGHT A3
#define LINE_RIGHT A4

#define LEFT_THRESH 800
#define MID_LEFT_THRESH 950
#define MID_RIGHT_THRESH 950
#define RIGHT_THRESH 1000

// Colour Sensor

#define COL_RED 3
#define COL_BLUE 4
#define COL_LDR A0
#define COL_DELAY 5   //in ms
#define COL_THRESH 20 //min difference between colours to define a detection

// Distance Sensor

#define DIST_TRIG 8
#define DIST_ECHO 11

// Photointerruptors

#define INT_1 2
#define INT_2 1

/**
 * Run all setup required for sensors
 */
void setupSensors()
{
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

/**
 * Run a debug test of the sensors
 */
void testSensors()
{
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
    if (col == 1)
    {
        Serial.println("Red");
    }
    else if (col == 0)
    {
        Serial.println("Blue");
    }
    else
    {
        Serial.println("Neither colour");
    }

    Serial.print("Distance Sensor: ");
    Serial.println(readDist(5));

    Serial.print("Photointerrupter 1: ");
    Serial.println(digitalRead(INT_1));
    Serial.print("Photointerrupter 2: ");
    Serial.println(digitalRead(INT_2));

    /*

    Serial.print(analogRead(LINE_LEFT));
    Serial.print(", ");
    Serial.print(analogRead(LINE_MID_LEFT));
    Serial.print(", ");
    Serial.print(analogRead(LINE_MID_RIGHT));
    Serial.print(", ");
    Serial.println(analogRead(LINE_RIGHT));
   */
}

bool readGo()
{
    return digitalRead(GO_BUTTON) == 0;
}

/**
 * Read the colour measured by the colour sensor
 * 
 * @returns 0 for Red, 1 for Blue, -1 for neither/indeterminable
 */
int readColour()
{
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
    if (red > blue + COL_THRESH)
    { // RED
        return 0;
    }
    if (blue > red + COL_THRESH)
    { // BLUE
        return 1;
    }
    return -1; // Neither
}

/**
 * Read a line sensor
 * 
 * @param sensor Number of the line sensor to read:
 *              0: Far left
 *              1: Mid left
 *              2: Mid right
 *              3: Far right
 * 
 * @return True if 'sensor' sees white, otherwise false, false if 'sensor' isn't valid 
 */
bool readLine(int sensor)
{
    if (sensor == 0)
    {
        return analogRead(LINE_LEFT) < LEFT_THRESH;
    }
    else if (sensor == 1)
    {
        return analogRead(LINE_MID_LEFT) < MID_LEFT_THRESH;
    }
    else if (sensor == 2)
    {
        return analogRead(LINE_MID_RIGHT) < MID_RIGHT_THRESH;
    }
    else if (sensor == 3)
    {
        return analogRead(LINE_RIGHT) < RIGHT_THRESH;
    }
    return false;
}

/**
 * Read the distance sensor.
 * 
 * @returns Distance in cm between 2 and 450cm, otherwise -1
 */
int readDistOnce()
{
    // defines variables
    long duration; // variable for the duration of sound wave travel
    int distance;  // variable for the distance measurement

    // Clears the trigPin condition
    digitalWrite(DIST_TRIG, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(DIST_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(DIST_TRIG, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(DIST_ECHO, HIGH, 2000);
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    if (distance > 100 || distance < 2)
    {
        return -1;
    }
    return distance;
}

int readDist(int repetitions)
{
    int tot = 0;
    int count = 0;
    int d;
    for (int i = 0; i < repetitions; i++)
    {
        d = readDistOnce();

        if (d > 0)
        {
            tot += d;
            count++;
        }
    }
    if (count == 0)
    {
        return -1;
    }
    return tot / count;
}
