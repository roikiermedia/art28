//
//                    _    ___    ___  
//       /\          | |  |__ \  / _ \ 
//      /  \    _ __ | |_    ) || (_) |
//     / /\ \  | '__|| __|  / /  > _ < 
//    / ____ \ | |   | |_  / /_ | (_) |
//   /_/    \_\|_|    \__||____| \___/ 
//
//  opensource artnet reciever for ws2812b on esp8266 & esp32 

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif 
#include <WiFiUdp.h>
#include <ArtnetWifi.h>
#include <FastLED.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

/******************************************************************************/
// CONFIGURATION
/******************************************************************************/

#define DATA_PIN    6
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    170
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          255
#define FRAMES_PER_SECOND   60

//Wifi settings
const char* NETWORK_NAME = "Chaostreff-Flensburg";
const char* NETWORK_PASSWORD = "Schnell33";

// Initialize the OLED display using Wire library (adress, SDA, SCL)
SSD1306  display(0x3c, D1, D2);

/******************************************************************************/
/******************************************************************************/

// Artnet settings
ArtnetWifi artnet;
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
const int numberOfChannels = NUM_LEDS * 3; // Total number of channels you want to receive (1 led = 3 channels)

// Check if we got all universes
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;

void setup() {
  delay(1000); // 1 second delay for recovery

  Serial.begin(115200);
  Serial.println();

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  WiFi.begin(NETWORK_NAME, NETWORK_PASSWORD);

  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 22, "Connecting to Network:");
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 34, NETWORK_NAME);
  display.display();

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    FastLED.delay(1000/FRAMES_PER_SECOND);
    Serial.print(".");
    runner(0x00FFFF); // run blue pixels up and down the strip while connecting
    FastLED.show();  
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 0, "ArtNet:");
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 17, IpAddressToString(WiFi.localIP()) );
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 35, WiFi.SSID() );
  display.drawString(64, 52, "ctfl.space/art28");
  display.display();

  // run green pixels on strip as a visual connection confirmation
  for (int i=0; i<=NUM_LEDS*1.5; i++) {
    runner(0x00FF00);
    FastLED.show();
    FastLED.delay(1000/FRAMES_PER_SECOND);
  }
  for (int i=0; i<=50; i++) {
    runner(0x000000);
    FastLED.show();
    FastLED.delay(1000/FRAMES_PER_SECOND/4);
  }

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  artnet.begin();
  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
}

void loop() {
  artnet.read();
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) {
  sendFrame = 1;
  // set brightness of the whole strip 
  if (universe == 15) {
    FastLED.setBrightness(data[0]);
  }

  // Store which universe has got in
  if ((universe - startUniverse) < maxUniverses) {
    universesReceived[universe - startUniverse] = 1;
  }

  for (int i = 0 ; i < maxUniverses ; i++) {
    if (universesReceived[i] == 0) {
      //Serial.println("Broke");
      sendFrame = 0;
      break;
    }
  }

  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / 3; i++) {
    int led = i + (universe - startUniverse) * (previousDataLength / 3);
    if (led < NUM_LEDS) {
      leds[i] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
  }
  previousDataLength = length;
  
  if (sendFrame) {
    FastLED.show();
    // Reset universeReceived to 0
    memset(universesReceived, 0, maxUniverses);
  }
}


// Preset Animations

void runner(CRGB clr) {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += clr;
}

// Helper functions

String IpAddressToString(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}
