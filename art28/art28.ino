#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif 
#include <WiFiUdp.h>
#include <ArtnetWifi.h>
#include <FastLED.h>

#define DATA_PIN    6
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    170
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND   60

//Wifi settings
const char* NETWORK_NAME = "Chaostreff-Flensburg";
const char* NETWORK_PASSWORD = "Schnell33";

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
  delay(3000); // 3 second delay for recovery

  Serial.begin(115200);
  Serial.println();

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  WiFi.begin(NETWORK_NAME, NETWORK_PASSWORD);

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
}

void loop() {
}


// Preset Animations

void runner(CRGB clr) {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += clr;
}

