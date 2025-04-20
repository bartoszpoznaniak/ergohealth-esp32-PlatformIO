#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_BMI270_Arduino_Library.h"
#include "ble.h"
// nclude <AccelStepper.h>

// Inicjalizacja czujnika BMI270
BMI270 myIMU;
float rawAccX, rawAccY, rawAccZ;

// Offset kalibracji
float offsetX = 0;
float offsetY = 0;

bool deviceConnected = false;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("Test BMI270 xxx");

  Wire.begin(21, 22); // Jawne określenie pinów SDA=21, SCL=22
  delay(100);         // Opóźnienie dla stabilizacji

  // Inicjalizacja BMI270
  bool success = myIMU.beginI2C();
  if (!success)
  {
    Serial.println("Error: BMI270 not connected, check wiring and I2C address!");
    while (1)
      ;
  }
  Serial.println("BMI270 connected!");

  setupBLE();

  Serial.println("BLE ready!");
}

void loop()
{
  rawAccX = myIMU.data.accelX;
  rawAccY = myIMU.data.accelY;

  // Wyświetlamy przez Serial z maksymalną precyzją
  Serial.printf("Serial: %f %f\n", rawAccX, rawAccY);

  loopBLE();

  // char txString[100];
  // // Wysyłamy przez BLE z maksymalną precyzją
  // sprintf(txString, "%.6f,%.6f", rawAccX, rawAccY);
}