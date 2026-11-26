#include <Wire.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#define SENSOR_PIN 4    // MQ-7 analog out

// bluetooth module on Serial2
#define RXD2 16  // module TX
#define TXD2 17  // module RX

// rough calibration bounds
#define MQ7_MIN 10
#define MQ7_MAX 10000

void setup() {
    Serial.begin(9600);
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

    dht.begin();
    pinMode(SENSOR_PIN, INPUT);

    Serial.println("WELCOME");
    Serial2.println("WELCOME");

    delay(2000);

    Serial.println("Project Title");
    Serial2.println("Project Title");
    delay(2000);

    Serial.println("Weather Monitoring System");
    Serial2.println("Weather Monitoring System");
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

    Serial.println("....................");
    Serial.print("Temperature: "); Serial.print(t); Serial.println("'C");
    Serial.print("Humidity: "); Serial.print(h); Serial.println("%");
    Serial.print("CO: "); Serial.print(gasPercentage); Serial.println("%");
    Serial.println("....................");

    Serial2.println("....................");
    Serial2.print("Temperature: "); Serial2.print(t); Serial2.println("'C");
    Serial2.print("Humidity: "); Serial2.print(h); Serial2.println("%");
    Serial2.print("CO: "); Serial2.print(gasPercentage); Serial2.println("%");
    Serial2.println("....................");

    delay(1000);
}
