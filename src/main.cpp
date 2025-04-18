#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_BMI270_Arduino_Library.h"
// nclude <AccelStepper.h>

// Inicjalizacja czujnika BMI270
BMI270 imu;
uint8_t i2cAddress = BMI2_I2C_PRIM_ADDR; // 0x68

// Offset kalibracji
float offsetX = 0;
float offsetY = 0;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("Test BMI270 xxx");

  Wire.begin();

  // Inicjalizacja BMI270
  while (imu.beginI2C(i2cAddress) != BMI2_OK)
  {
    Serial.println("Error: BMI270 not connected, check wiring and I2C address!");
    delay(1000);
  }
  Serial.println("BMI270 connected!");

  // Serial.print("Inicjalizacja BMI270: ");
  // Serial.println(success ? "OK" : "BŁĄD");
}

void loop()
{
  Serial.print("AccelX: \n");
  // Serial.println(myIMU.data.accelX);

  if (imu.getSensorData() == BMI2_OK)
  {
    // Przetwarzanie danych
    float rawAccX = imu.data.accelX;
    float rawAccY = imu.data.accelY;
    float rawAccZ = imu.data.accelZ;

    // Obliczanie kątów
    float angleX = atan2(rawAccX, sqrt(rawAccY * rawAccY + rawAccZ * rawAccZ)) * 180.0 / PI - offsetX;
    float angleY = -atan2(rawAccY, sqrt(rawAccX * rawAccX + rawAccZ * rawAccZ)) * 180.0 / PI - offsetY;

    Serial.printf("%f %f %f \n", rawAccX, rawAccY, rawAccZ);
  }
  delay(100);
}