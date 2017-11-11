#include <Arduino.h>
#include "font.h"

#include <Wire.h>
#include "SSD1306.h"

#include <SPI.h>
#include <LoRa.h>

#define OLED_ADDR 0x3C
#define OLED_SDA  4
#define OLED_SCL  15
#define OLED_RST  16

SSD1306 display(OLED_ADDR, OLED_SDA, OLED_SCL);

#define SX1278_SCK  5
#define SX1278_MISO 19
#define SX1278_MOSI 27
#define SX1278_CS   18
#define SX1278_RST  14
#define SX1278_IRQ  26

#define BAND 434500000.00
#define spreadingFactor 9
#define SignalBandwidth 31.25E3
#define preambleLength 8
#define codingRateDenominator 8
  
void setup()
{
  // Reset the display
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);

  // Initialize and wait for UART
  Serial.begin(115200);
  while (!Serial);

  // Initialize the display
  display.init();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_CENTER);

  // Start LoRa
  SPI.begin(SX1278_SCK, SX1278_MISO, SX1278_MOSI, SX1278_CS);
  LoRa.setPins(SX1278_CS, SX1278_RST, SX1278_IRQ);
  
  if(!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while(1);
  }

  Serial.println("LoRa started successful!");
  
  Serial.print("LoRa Frequency: ");
  Serial.print(BAND/1000000);
  Serial.println(" MHz");
  
  Serial.print("LoRa Spreading Factor: ");
  Serial.println(spreadingFactor);
  LoRa.setSpreadingFactor(spreadingFactor);
  
  Serial.print("LoRa Signal Bandwidth: ");
  Serial.println(SignalBandwidth);
  LoRa.setSignalBandwidth(SignalBandwidth);

  LoRa.setCodingRate4(codingRateDenominator);

  LoRa.setPreambleLength(preambleLength);

  // Print an empty line
  Serial.println();
  
  // Wait for a second
  delay(1000);
}

void loop()
{
  int packetSize = LoRa.parsePacket();
  if(packetSize)
  {
    Serial.println("Received packet!");
    
    display.clear();
    display.setFont(Lato_Hairline_16);
    display.drawString(63, 0, "RECEIVED:");
    display.display();
    
    while(LoRa.available())
    {
      String data = LoRa.readString();
      
      Serial.print("Content: ");
      Serial.println(data);
      
      display.drawString(63, 21, data);
      display.display();
    }
    
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
    Serial.print("SNR: ");
    Serial.println(LoRa.packetSnr());
    Serial.println();
    
    display.drawString(63, 42, (String)LoRa.packetRssi() + "dB (" + (String)LoRa.packetSnr() +"dB)");
    display.display();
  }
}
