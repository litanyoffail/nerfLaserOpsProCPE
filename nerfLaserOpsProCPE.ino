#include <Adafruit_CircuitPlayground.h>
#include <Adafruit_Circuit_Playground.h>

#define myReceiver CircuitPlayground.irReceiver
#define myDecoder CircuitPlayground.irDecoder

/* IR hashes for Nerf LaserOps Pro standard game */
#define AP_PURPLE 0x67228B44
#define AP_RED 0x78653B0E
#define AP_BLUE 0x2FFEA610

#define DB_PURPLE 0xD303E9B8
#define DB_RED 0xE4469982
#define DB_BLUE 0x9BE00484

#define PURPLE 0x00FF00FF
#define RED 0x00FF0000
#define GREEN 0x0000FF00
#define BLUE 0x000000FF

const char* teamName[] = {"PURPLE", "RED", "BLUE"};
const uint32_t teamColor[] = {PURPLE, RED, BLUE};

#define HOLD_DELAY_VALID_HIT 250
#define HOLD_DELAY_TEAM_SWITCH 500
#define ANIMATION_DELAY_SETUP 180
#define ANIMATION_DELAY_TEAM_SWITCH 180
#define ANIMATION_DELAY_RESET 200
#define ANIMATION_DELAY_HIT_POINT 200
#define ANIMATION_DELAY_NO_HIT_POINT 1000

Adafruit_CPlay_NeoPixel strip = Adafruit_CPlay_NeoPixel(10, CPLAY_NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

// Purple Team = 0
// Red Team = 1
// Blue Team = 2
int ownerTeam = 0;

// Hit Points and Damage
#define HIT_POINT_MAX 10
int hitPoint = HIT_POINT_MAX;

// Hits will only be valid from opposing team if Red or Blue, or any Purple
// Prevents Red/Blue from affecting Purple and vice versa to copy stock behaviour.
// Maybe there is a game mode that can be tied to modyfing this behaviour somehow?
bool validHit() {
  const uint32_t alphaPoint[] = {AP_PURPLE, AP_BLUE, AP_RED};
  const uint32_t deltaBurst[] = {DB_PURPLE, DB_BLUE, AP_RED};

  return myDecoder.protocolNum == UNKNOWN &&
         (myDecoder.value == alphaPoint[ownerTeam] ||
          myDecoder.value == deltaBurst[ownerTeam]);
}

void validHitResponse() {
  --hitPoint;

  Serial.print(F("VALID HIT "));
  Serial.print(teamName[ownerTeam]);
  Serial.print(F(" 0x"));
  Serial.println(myDecoder.value, HEX);

  setLEDColor(teamColor[ownerTeam], 0);
  delay(HOLD_DELAY_VALID_HIT);
  setLEDNone();
}

// Cycle ownerTeam variable between teams and show a boot-up animation to indicate ownerTeam color when Right Button is pressed.
void teamSwitchButton() {
  setLEDNone();
  if (++ownerTeam > 2) {
    ownerTeam = 0;
  }
  setLEDColor(teamColor[ownerTeam], ANIMATION_DELAY_TEAM_SWITCH);
  delay(HOLD_DELAY_TEAM_SWITCH);
  hitPointLEDGreen();
}

// Reset the unit to full health on current ownerTeam when Left Button is pressed.
void resetButton() {
  hitPoint = HIT_POINT_MAX;
  setLEDNone();
  setLEDColor(teamColor[ownerTeam], ANIMATION_DELAY_RESET);
  hitPointLEDGreen();
}

// set all NeoPixels to the given color
void setLEDColor(uint32_t color, int animation_delay)
{
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;

  for (int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, r, g, b);
    strip.show();
    delay(animation_delay);
  }
}

void showHitPointLED(int hp) {
  setLEDNone();
  if (hp > 0) {
      for (int i=0; i<hp; i++) {
        strip.setPixelColor(i, 0x00, 0xFF, 0x00);
        strip.show();
        delay(ANIMATION_DELAY_HIT_POINT);
      }
  } else {
    for (int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 0xFF, 0xFF, 0xFF);
      strip.show();
      delay(ANIMATION_DELAY_NO_HIT_POINT);
    }
    setLEDNone();
  }
}

void setLEDNone() {
  strip.clear();
  strip.show();
}

void setup() {
  CircuitPlayground.begin();
  Serial.begin(9600);
//  while (!Serial); //delay to wait for serial port
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals"));
  strip.begin();
  strip.setBrightness(42);
  strip.clear();
  strip.show();
  setLEDColor(PURPLE, ANIMATION_DELAY_SETUP);
  setLEDColor(GREEN, ANIMATION_DELAY_SETUP);
}

void loop() {
    // Activate IR Receiver
  if (myReceiver.getResults()) {
    myDecoder.decode();
    if (validHit()) {
      validHitResponse();
      showHitPointLED(hitPoint);
    }
    else {
      Serial.print(F("INVALID HIT 0x"));
      Serial.println(myDecoder.value, HEX);
    }
  }
  myReceiver.enableIRIn();

  if (CircuitPlayground.rightButton()) {
    teamSwitchButton();
  }

  if (CircuitPlayground.leftButton()) {
    resetButton();
  }
}
