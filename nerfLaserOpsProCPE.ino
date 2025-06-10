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

#define PURPLE 0x000A000A
#define RED 0x000A0000
#define GREEN 0x00000A00
#define BLUE 0x0000000A

const char* teamName[] = {"PURPLE", "RED", "BLUE"};
const uint32_t teamColor[] = {PURPLE, RED, BLUE};

Adafruit_CPlay_NeoPixel strip = Adafruit_CPlay_NeoPixel(10, CPLAY_NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

// Purple Team = 0
// Red Team = 1
// Blue Team = 2
int teamOwner = 0;
int teamEnemy = 0;

// Hit Point Config
#define HIT_POINT_BASE 10
int HIT_POINT_MULTIPLIER = 1;
int hitPoint = HIT_POINT_BASE * HIT_POINT_MULTIPLIER;

// Hits will only be valid from opposing team if Red or Blue, or any Purple
// Prevents Red/Blue from affecting Purple and vice versa to copy stock behaviour.
// Maybe there is a game mode that can be tied to modyfing this behaviour somehow?
bool validHit() {
  myDecoder.decode();
  const uint32_t alphaPoint[] = {AP_PURPLE, AP_RED, AP_BLUE};
  const uint32_t deltaBurst[] = {DB_PURPLE, DB_RED, AP_BLUE};

  return myDecoder.protocolNum == UNKNOWN && 
    (myDecoder.value == alphaPoint[teamEnemy] ||
    myDecoder.value == deltaBurst[teamEnemy]) &&
    hitPoint > 0;
}

void validHitResponse() {
  --hitPoint;
  Serial.print(F("VALID HIT "));
  Serial.print(teamName[teamEnemy]);
  Serial.print(F(" 0x"));
  Serial.println(myDecoder.value, HEX);
  clearLED();
  setLED(teamColor[teamEnemy], 0);
  delay(240);
  showHitPoint();
}

// Cycle teamOwner variable between teams and show a boot-up animation to indicate teamOwner color when Right Button is pressed.
void teamSwitchButton() {
  hitPoint = HIT_POINT_BASE * HIT_POINT_MULTIPLIER;
  if (++teamOwner > 2) {
    teamOwner = 0;
  }
  if (--teamEnemy < 0) {
    teamEnemy = 2;
  }
  clearLED();
  setLED(teamColor[teamOwner], 100);
  showHitPoint();
}

// Cycle between 10/20/30 Hit Points.
void modeButton() {
  if (++HIT_POINT_MULTIPLIER < 3) {
    HIT_POINT_MULTIPLIER = 1;
  }
}

//Show HP on Neopixel LED
void showHitPoint() {
  clearLED();
  for (int i=0; i<hitPoint; i++) {
    strip.setPixelColor(i, 0x00, 0x0A, 0x00);
  }
  strip.show();
}

//Set all Neopixel LED, with optional animation delay.
void setLED (uint32_t color, int animation_delay) {
  uint8_t r = (color >> 16) & 0x0A;
  uint8_t g = (color >> 8) & 0x0A;
  uint8_t b = color & 0x0A;
  for (int i=0; i<hitPoint; i++) { 
    strip.setPixelColor(i, r, g, b);
    strip.show();
    delay(animation_delay);
  }
}

//Clears all LEDs.
void clearLED() {
  strip.clear();
  strip.show();
}

//Loops when HP is 0, and you can hold the teamSwitchButton to reset HP and escape the loop.
void dead() {
  clearLED();
  for (int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, 0x0A, 0x0A, 0x00);
    if (CircuitPlayground.rightButton()) {
      teamSwitchButton();
      return;
    }
    strip.show();
    delay(100);
  }
}

void setup() {
  CircuitPlayground.begin();
  Serial.begin(9600);
//  while (!Serial); //delay to wait for serial port
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals"));
  strip.begin();
  clearLED();
  setLED(teamColor[teamOwner], 100);
  showHitPoint();
}

void loop() {
  //Loop death animation while HP is 0
  if (hitPoint <= 0) {
    dead();
  }

  //Initialize IR Receiver once it gets a signal and reset IR Receiver.
  if (myReceiver.getResults()) {
    if (validHit() == true) {
      validHitResponse();
    }
    else {
      Serial.print(F("INVALID HIT 0x"));
      Serial.println(myDecoder.value, HEX);
    }
    myReceiver.enableIRIn();
  }

  if (CircuitPlayground.rightButton()) {
    teamSwitchButton();
  }

  if (CircuitPlayground.leftButton()) {
    modeButton();
  }
}
