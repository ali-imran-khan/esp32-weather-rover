# esp32-weather-rover

An ESP32 robot car that drives around avoiding obstacles while logging temperature, humidity and carbon monoxide. Made for the Embedded Systems Workshop course at COMSATS Lahore as a group project. The three folders are the three stages it went through.

| folder | stage |
|---|---|
| `v1-bluetooth-station/` | midterm: static station. DHT11 + MQ-7, readings sent to a phone over a Bluetooth serial module |
| `v2-blynk-rover/` | final: on wheels. HC-SR04 obstacle avoidance with an L298N, readings pushed to a Blynk dashboard over Wi-Fi |
| `v3-servo-scan/` | final, improved: ultrasonic sensor on a servo. When blocked it looks left and right and turns toward the clearer side (or spins 180° if both are blocked). Data goes over Bluetooth |

## Parts

ESP32 DevKit, DHT11, MQ-7, HC-SR04, micro servo, L298N, 2 DC gear motors, Bluetooth serial module, battery pack.

## Run

Arduino IDE with ESP32 board support. Libraries: `DHT sensor library`, `ESP32Servo` (v3), `Blynk` (v2). For v2, copy `secrets.example.h` to `secrets.h` and fill it in.

## Caveats

- The MQ-7 output is used as a raw percentage of a rough range, not calibrated ppm. It needs its heater cycle and a burn-in for real CO readings.
- Turns are timed with `delay()`, so turn angle depends on battery level.
