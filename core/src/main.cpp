#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiType.h>
#include <WiFiUdp.h>

#define RED D1
#define GREEN D2
#define BLUE D3
#define RESET_LED setColor(0, 0, 0)

static uint8_t red = 0;
static uint8_t green = 0;
static uint8_t blue = 0;

const char *ssid = "busya";
const char *password = "0677170801";
const int udpPort = 12345;

WiFiUDP udp;

void parseString(String input) {
  int comma1 = input.indexOf(',');
  int comma2 = input.indexOf(',', comma1 + 1);

  if (comma1 != -1 && comma2 != -1) {
    String strValue1 = input.substring(0, comma1);
    String strValue2 = input.substring(comma1 + 1, comma2);
    String strValue3 = input.substring(comma2 + 1);

    red = strValue1.toInt();
    green = strValue2.toInt();
    blue = strValue3.toInt();
  } else {
    Serial.println("Invalid input string");
  }
}

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

  RESET_LED;

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  udp.begin(udpPort);
  Serial.printf("UDP server started on port %d\n", udpPort);
  Serial.println(WiFi.localIP().toString());
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char packetBuffer[255];
    int len = udp.read(packetBuffer, sizeof(packetBuffer));

    if (len > 0) {
      packetBuffer[len] = 0;
      // TODO: add word to packet: example: color: 255, 255, 255; mode: blink;
      // etc...
      parseString(packetBuffer);
      setColor(red, green, blue);
    }
  }
}