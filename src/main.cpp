#include <Arduino.h>

#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include "MCP3008.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>

#include <SPI.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//define pin connections
#define CS_PIN D2
#define CLOCK_PIN D3
#define MOSI_PIN D4
#define MISO_PIN D6

//Constants
WiFiUDP Udp;
MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, D7, NEO_GRB + NEO_KHZ800);
// SHould not be more than 256 in order to minimize packet framentation (1 short int is packed in two bytes, giving a max packet size og 512 for buffer size 256)
const int BUFFER_SIZE = 256;
const int SLEEP_INTERVAL_MS = 10;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pixels.begin(); // This initializes the NeoPixel library.

  // Set pixel to red before connecting
  pixels.setPixelColor(0, pixels.Color(150,0,0));
  pixels.show();

  /*
  for(uint8_t t = 6; t > 0; t--) {
      Serial.printf("[SETUP] WAIT %d...\n", t);
      Serial.flush();
      delay(1000);
  }
  */
  /*
  WiFiMulti.addAP("1 er sejere end 4", "elisabeth");

  while(WiFiMulti.run() != WL_CONNECTED) {
    Serial.println("Not connected to wifi. Wating");
    pixels.setPixelColor(0, pixels.Color(255,0,0));
    delay(500);
    pixels.setPixelColor(0, pixels.Color(150,0,0));
    delay(500);
  }
  */
  WiFi.begin("1 er sejere end 4", "elisabeth");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Not connected to wifi. Wating");
    pixels.setPixelColor(0, pixels.Color(255,0,0));
    delay(500);
    pixels.setPixelColor(0, pixels.Color(150,0,0));
    delay(500);
  }
  Serial.print("WiFi concceted. Local IP:");
  Serial.println(WiFi.localIP());
  // Change LED to green when connected
  pixels.setPixelColor(0, pixels.Color(0,150,0));
  pixels.show();
}

// Convert an array of 10 short integeres to an array of 20 bytes.
// This funciton allocates 20 bytes from heap. Please remember to free the memory
uint8_t* shortToBytes(short* shorts, int len) {
  uint8_t *bytes = (uint8_t *)malloc(len * 2);

  for (int i = 0; i < len; i++) {
    bytes[i * 2] = (uint8_t) (shorts[i] & 0x00FF);
    bytes[(i * 2) + 1] = (uint8_t) (shorts[i] >> 8);
  }
  return bytes;
}


char  replyPacekt[] = "Hi there! Got the message :-)";
short buffer[BUFFER_SIZE];
int curBufferPos = 0 ;
void loop() {
  double vRef        = 3.30;

  short val = adc.readADC(0); // read Channel 0 from MCP3008 ADC (pin 1)

  //Serial.println(val);
  if (curBufferPos == BUFFER_SIZE) {
    Serial.println("Transmitting over UDP");
    Udp.beginPacket("192.168.1.17", 10001);
    uint8_t *b = shortToBytes(buffer, BUFFER_SIZE);
    Udp.write(b, BUFFER_SIZE * 2);
    Udp.endPacket();
    free(b);
    curBufferPos = 0;
  }

  buffer[curBufferPos] = val;
  curBufferPos++;
  delay(SLEEP_INTERVAL_MS);
}
