#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiType.h>
#include <WiFiUdp.h>

#define RED D1
#define GREEN D2
#define BLUE D3

const char *ssid = "busya";
const char *password = "0677170801";
const int udpPort = 12345;

WiFiUDP udp;

void setColor(uint8_t red, uint8_t green, uint8_t blue) {
  analogWrite(RED, red);
  analogWrite(GREEN, green);
  analogWrite(BLUE, blue);
}

void setup() {
  Serial.begin(115200);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  analogWrite(RED, 0);
  analogWrite(GREEN, 0);
  analogWrite(BLUE, 0);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  udp.begin(udpPort);
  Serial.printf("UDP server started on port %d\n", udpPort);
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize,
                  udp.remoteIP().toString().c_str(), udp.remotePort());

    char packetBuffer[255];
    int len = udp.read(packetBuffer, sizeof(packetBuffer));

    if (len > 0) {
      packetBuffer[len] = 0;
      Serial.printf("Received message: %s\n", packetBuffer);
      // TODO: add parser to message
      //  call color changer method
    }

    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.printf("Hello from NodeMCU!");
    udp.endPacket();
  }
}