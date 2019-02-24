#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include <U8x8lib.h>

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    915E6

void println(String str) {
  Serial.println(str);
  u8x8.println(str);
}

void printPacketData(char * data) {
  char * token = strtok(data, " ,");
  for (int i = 0; i < 3 && token != NULL; i++) {
    println(token);
    token = strtok(NULL, " ,");
  }
}

void cbk(int packetSize) {
  u8x8.clear();
  println("RSSI " + String(LoRa.packetRssi(), DEC));
  char * packetData = new char[packetSize + 1];
  LoRa.readBytes(packetData, packetSize);
  packetData[packetSize] = '\0';
  printPacketData(packetData);
  delete[] packetData;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(915E6)) {
    println("LoRa failed!");
    while (1);
  }

  LoRa.receive();
   
  u8x8.begin();
  u8x8.setFont(u8x8_font_5x7_f);

  println("RX ready");

  delay(1500);
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) { cbk(packetSize);  }
  delay(10);
}