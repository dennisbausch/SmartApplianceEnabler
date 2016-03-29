/*
 * Copyright (C) 2015 Axel Müller <axel.mueller@avanux.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * This Arduino IDE sketch is for the ESP8266-ESP01 a low-cost Wi-Fi chip with full TCP/IP stack
 * and microcontroller capability produced by Shanghai-based Chinese manufacturer, Espressif.
 * The sketch is used read pulses of an electricity meter and forward each pulse as an UDP packet via Wi-Fi
 * to the SmartApplianceEnabler
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

/* 1. Create "Credentials" tab
 * 2. Add the 2 lines below and update ssid value according to your Wifi network
 * // SSID of wifi network - !!! case sensitive !!!
 * const char* ssid = "MySSID";
 */
extern const char* ssid;
/* 3. Add the 2 lines below and update password value according to your Wifi network
 * // Password of wifi network - !!! case sensitive !!!
 * const char* password = "myPassword";
 */
extern const char* password;
// the IP address of the SmartApplianceEnabler receiving the UDP packets
IPAddress saeIpAddress(192, 168, 69, 5);
// the UDP port of the SmartApplianceEnabler receiving the UDP packets
unsigned int saePort = 9999;
// the local port used for sending the UDP packets
unsigned int localPort = 9999;
// the ID of the appliance for which the S0ElectricityMeterNetworked is configured in SmartApplianceEnabler
const char* applianceId = "F-00000001-000000000001-00";
// the GPIO pin of the ESP8266-ESP01 used to read S0 pulses
const int meterPin = 2;
// the previous state of the GPIO pin
int previousStatus = HIGH;
// the UDP packet counter allows detection of dropped packets by receiver
unsigned long counter = 0;

WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  delay(10);

  // This enables the internal pullup resistor so that the default state is high rather than floating.
  pinMode(meterPin, INPUT_PULLUP);
  
  // Connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Print the IP address
  Serial.println(WiFi.localIP());

  Serial.println("Opening UDP port ...");
  udp.begin(localPort);
}

void loop() {
  int status = digitalRead(meterPin);
  // because of the pullup resistor, the pin state is reversed: button pressed = LOW, button released = HIGH
  if (status == LOW && previousStatus == HIGH) {
    Serial.println("Sending packet...");
    
    char buffer[10+1];
    ltoa(++counter, buffer, 10);
    
    udp.beginPacket(saeIpAddress, saePort);
    udp.write(applianceId);
    udp.write(":");
    udp.write(buffer);
    udp.endPacket();
    
    // de-bouncing is required:
    // 2016-03-28 16:28:26,367 DEBUG Received UDP packet: F-00000001-000000000001-00:355
    // 2016-03-28 16:28:26,401 DEBUG Received UDP packet: F-00000001-000000000001-00:356
    // delay has to be shorte than impuls duration (90ms for DRS155B)
    delay(70);
  }
  previousStatus = status;
}
