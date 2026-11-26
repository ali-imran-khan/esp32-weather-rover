#include "secrets.h"  // BLYNK_* defines + WIFI_SSID / WIFI_PASS, see secrets.example.h

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

char auth[] = BLYNK_AUTH_TOKEN;
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

BlynkTimer timer;

// Ultrasonic sensor pins
#define echo 18
#define trigger 5

// Motor driver pins
#define in1 22
#define in2 23
#define in3 32
#define in4 33

// DHT sensor pins and type
#define DHTPIN_ENV 2     // ambient
#define DHTTYPE DHT11
DHT dhtEnv(DHTPIN_ENV, DHTTYPE);

// MQ-7 sensor pin
#define MQ7_PIN 34

long duration;
int distance;

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
  Blynk.begin(auth, ssid, password);

  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(MQ7_PIN, INPUT);

  dhtEnv.begin();

  timer.setInterval(1000L, sendSensorDataToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();

  // Robot obstacle avoidance logic
  moveForward();
  int frontDistance = getDistance();
  Serial.print("Front Distance: ");
  Serial.println(frontDistance);

  if (frontDistance < 20) {
    stopMotors();
    Serial.println("Obstacle detected. Turning Right.");
    rotateRight();
    delay(1000);
  }
}

// send data to Blynk
void sendSensorDataToBlynk() {
  float temperature = dhtEnv.readTemperature();
  float humidity = dhtEnv.readHumidity();
  int coLevel = analogRead(MQ7_PIN);

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Convert CO level to percentage
  float coPercentage = map(coLevel, 0, 4095, 0, 100);

  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, coPercentage);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, CO: ");
  Serial.print(coPercentage);
  Serial.println(" %");
}

// calculate distance using HC-SR04
int getDistance() {
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
  return distance;
}

// move motors forward
void moveForward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

// stop motors
void stopMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

// rotate the robot right
void rotateRight() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}
