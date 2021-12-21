#include <FastLED.h>

#ifdef __AVR_ATtiny85__
#include <EEPROM.h>
#define LED_BUILTIN 1
#define FLASH_EEPROM false
#else
#include <FlashAsEEPROM.h>
#define FLASH_EEPROM true
#endif

#ifdef ARDUINO_TRINKET_M0
#include <Adafruit_DotStar.h>

Adafruit_DotStar dotstar = Adafruit_DotStar(1, INTERNAL_DS_DATA, INTERNAL_DS_CLK, DOTSTAR_BGR);
#define LEDS_PIN 2
#else

// Which pins your NeoPixels are connected to
#define LEDS_PIN 4

#endif

// How many LEDS you have. 24 for xmas advent, 9 for menorah, etc.
#define NUM_LEDS 24

// Want one of them to be white?
#define THE_WHITE_ONE (NUM_LEDS - 1)

// How many milliseconds between twinkling
#define DELAY_MS 1000

// What percentage chance a chosen light has of being on
#define ACTIVITY 20

// Where to write the next value
#define STORAGE_SLOT 25

#define STAR_COLOR CHSV(24, 150, 255)

uint8_t RandomHue() {
  switch (random(12)) {
    case 0 ... 2:
      return 52; // reddish yellow
    case 3 ... 5:
      return HUE_RED;
    case 6 ... 8:
      return 28; // reddish orange
    case 9:
      return HUE_BLUE;
    case 10:
      return HUE_GREEN;
    case 11:
      return 204; // reddish purple
  }
}

CRGB leds[NUM_LEDS];
int lastLightOn;

void setup() {
  FastLED.addLeds<WS2812B, LEDS_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(64);

  pinMode(LED_BUILTIN, OUTPUT);

#ifdef ARDUINO_TRINKET_M0
  dotstar.begin();
  dotstar.show();
#endif

#if defined(DEBUG)
  lastLightOn = NUM_LEDS - 1;
#elif defined(FLASH_AS_EEPROM_h)
  if (EEPROM.isValid()) {
    lastLightOn = EEPROM.read(STORAGE_SLOT);
  } else {
    lastLightOn = 0;
  }
  EEPROM.update(STORAGE_SLOT, (lastLightOn + 1) % NUM_LEDS);
  EEPROM.commit();
#else
  lastLightOn = EEPROM.read(STORAGE_SLOT);
  EEPROM.update(STORAGE_SLOT, (lastLightOn + 1) % NUM_LEDS);
#endif
}

bool strandUpdate() {
  static unsigned long nextEventMillis = 0;
  unsigned long now = millis();
  static int group = 0;

  if (now < nextEventMillis) {
    return false;
  }

  int twinkler = random(lastLightOn + 1);
  // Make sure it's got the right things on and off
  for (int pos = 0; pos < NUM_LEDS; ++pos) {
    uint8_t hue = RandomHue();
    uint8_t saturation = 255;
    uint8_t value = 255;
    if (pos <= lastLightOn) {
      if (pos == THE_WHITE_ONE) {
        saturation = 30;
      } 
      if ((pos == twinkler) || !(leds[pos])) {
        if ((random(100) > ACTIVITY) && (pos != lastLightOn)) {
          value = 100;
        }
        leds[pos] = CHSV(hue, saturation, value);
      }
    } else {
      leds[pos] = 0;
    }
  }
  nextEventMillis = now + DELAY_MS;
  return true;
}

void loop() {
  bool update = false;

  update |= strandUpdate();

  if (update) {
    FastLED.show();
  }

  // blink the heart for a little bit
  if (millis() < 8 * 1000) {
    bool on = (millis() % 1000) < 500;
    digitalWrite(LED_BUILTIN, on);
  }
}
