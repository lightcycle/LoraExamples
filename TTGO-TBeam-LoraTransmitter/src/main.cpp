#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <TinyGPS++.h>

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISnO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND  915E6

unsigned int counter = 0;

String rssi = "RSSI --";
String packSize = "--";
String packet; 

TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 12, 15); //17-TX 18-RX
  while (!Serial);
  Serial.println();
  Serial.println("LoRa Sender Test");
  
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(62.5E3);
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("init ok");
   
  delay(1500);
}

static void smartDelay(unsigned long ms)                
{
  unsigned long start = millis();
  do {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}

void loop() {
  char buffer[80];
  if (gps.location.isValid()) {
    sprintf(buffer, "%d %.4f,%.4f", counter, gps.location.lat(), gps.location.lng());
  } else {
    sprintf(buffer, "%d", counter);
  }
  String message = String(buffer);

  Serial.println(message);

  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();

  counter++;

  smartDelay(5000);
}