#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_BMI270_Arduino_Library.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
// nclude <AccelStepper.h>

// Inicjalizacja czujnika BMI270
BMI270 imu;
uint8_t i2cAddress = BMI2_I2C_PRIM_ADDR; // 0x68

// Offset kalibracji
float offsetX = 0;
float offsetY = 0;

// UUID dla serwisu i charakterystyki BLE
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// Callback dla połączeń BLE
class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
  }
};

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

  // Inicjalizacja BLE
  BLEDevice::init("ESP32-BMI270");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();

  Serial.println("BLE ready!");
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

    if (deviceConnected)
    {
      // Formatujemy dane do wysłania przez BLE
      char txString[50];
      sprintf(txString, "%.2f,%.2f,%.2f", rawAccX, rawAccY, rawAccZ);

      pCharacteristic->setValue(txString);
      pCharacteristic->notify();
    }
  }
  delay(100);
}