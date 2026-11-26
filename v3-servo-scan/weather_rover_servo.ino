#include <Wire.h>
#include "DHT.h"
#include <ESP32Servo.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#define SENSOR_PIN 4    // MQ-7 analog out

// bluetooth module on Serial2
#define RXD2 16  // module TX
#define TXD2 17  // module RX

#define TRIG_PIN 5
#define ECHO_PIN 18
#define LEFT_MOTOR_EN 25
#define RIGHT_MOTOR_EN 21
#define LEFT_MOTOR_IN1 22
#define LEFT_MOTOR_IN2 23
#define RIGHT_MOTOR_IN3 32
#define RIGHT_MOTOR_IN4 33
#define SERVO_PIN 19

Servo servoMotor;

// rough MQ-7 bounds
#define MQ7_MIN 10
#define MQ7_MAX 10000

// Distance and safe distance
long duration;
int distance;
const int SAFE_DISTANCE = 20; // cm

// Servo angles
const int CENTER_ANGLE = 90;
const int LEFT_ANGLE = 135;
const int RIGHT_ANGLE = 45;

void setup() {
    Serial.begin(9600);
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

    dht.begin();
    pinMode(SENSOR_PIN, INPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    pinMode(LEFT_MOTOR_EN, OUTPUT);
    pinMode(RIGHT_MOTOR_EN, OUTPUT);
    pinMode(LEFT_MOTOR_IN1, OUTPUT);
    pinMode(LEFT_MOTOR_IN2, OUTPUT);
    pinMode(RIGHT_MOTOR_IN3, OUTPUT);
    pinMode(RIGHT_MOTOR_IN4, OUTPUT);

    servoMotor.attach(SERVO_PIN);
    servoMotor.write(CENTER_ANGLE);

    digitalWrite(LEFT_MOTOR_EN, HIGH);
    digitalWrite(RIGHT_MOTOR_EN, HIGH);

    Serial.println("WELCOME");
    Serial2.println("WELCOME");
    delay(2000);
    Serial.println("Project Title");
    Serial2.println("Project Title");
    delay(2000);
    Serial.println("Weather Monitoring and Obstacle Avoidance System");
    Serial2.println("Weather Monitoring and Obstacle Avoidance System");
    delay(2000);
}

void loop() {
    int h = dht.readHumidity();
    int t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        Serial2.println(F("Failed to read from DHT sensor!"));
        return;
    }

    int gas = analogRead(SENSOR_PIN);

    if (gas < MQ7_MIN || gas > MQ7_MAX) {
        Serial.println(F("Invalid reading from MQ-7 sensor!"));
        Serial2.println(F("Invalid reading from MQ-7 sensor!"));
        return;
    }

    float gasPercentage = ((float)(gas - MQ7_MIN) / (MQ7_MAX - MQ7_MIN)) * 100.0;

    Serial.println("Weather Data...");
    Serial.print("Temperature: "); Serial.print(t); Serial.println("'C");
    Serial.print("Humidity: "); Serial.print(h); Serial.println("%");
    Serial.print("CO: "); Serial.print(gasPercentage); Serial.println("%");
    Serial.println("....................");

    Serial2.println("Weather Data...");
    Serial2.print("Temperature: "); Serial2.print(t); Serial2.println("'C");
    Serial2.print("Humidity: "); Serial2.print(h); Serial2.println("%");
    Serial2.print("CO: "); Serial2.print(gasPercentage); Serial2.println("%");
    Serial2.println("....................");

    // Check forward distance for obstacle avoidance
    servoMotor.write(CENTER_ANGLE);
    delay(500);
    int distanceForward = getDistance();

    if (distanceForward > SAFE_DISTANCE) {
        moveForward();
    } else {
        stopMotors(); // blocked: look left and right
        delay(500);

        servoMotor.write(LEFT_ANGLE);
        delay(500);
        int distanceLeft = getDistance();

        servoMotor.write(RIGHT_ANGLE);
        delay(500);
        int distanceRight = getDistance();

        // Decide direction based on distances
        if (distanceLeft > distanceRight && distanceLeft > SAFE_DISTANCE) {
            turnLeft();
        } else if (distanceRight > SAFE_DISTANCE) {
            turnRight();
        } else {
            // No path available, turn 180 degrees
            turnAround();
        }
    }

    delay(1000);
}

// get distance from HC-SR04
int getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2; // Convert to cm
    return distance;
}

// move forward
void moveForward() {
    digitalWrite(LEFT_MOTOR_IN1, HIGH);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
    digitalWrite(RIGHT_MOTOR_IN3, HIGH);
    digitalWrite(RIGHT_MOTOR_IN4, LOW);
    Serial.println("Moving forward...");
}

// turn left
void turnLeft() {
    digitalWrite(LEFT_MOTOR_IN1, LOW);
    digitalWrite(LEFT_MOTOR_IN2, HIGH);
    digitalWrite(RIGHT_MOTOR_IN3, HIGH);
    digitalWrite(RIGHT_MOTOR_IN4, LOW);
    delay(500); // tuned by hand
    stopMotors();
    Serial.println("Turning left...");
}

// turn right
void turnRight() {
    digitalWrite(LEFT_MOTOR_IN1, HIGH);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
    digitalWrite(RIGHT_MOTOR_IN3, LOW);
    digitalWrite(RIGHT_MOTOR_IN4, HIGH);
    delay(500); // tuned by hand
    stopMotors();
    Serial.println("Turning right...");
}

// turn 180 degrees (spin in place)
void turnAround() {
    Serial.println("Turning 180 degrees...");
    // Spin both wheels in opposite directions
    digitalWrite(LEFT_MOTOR_IN1, HIGH);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
    digitalWrite(RIGHT_MOTOR_IN3, LOW);
    digitalWrite(RIGHT_MOTOR_IN4, HIGH);
    delay(1000); // ~180 deg on our chassis
    stopMotors();
}

// stop the motors
void stopMotors() {
    digitalWrite(LEFT_MOTOR_IN1, LOW);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
    digitalWrite(RIGHT_MOTOR_IN3, LOW);
    digitalWrite(RIGHT_MOTOR_IN4, LOW);
    Serial.println("Motors stopped.");
}
