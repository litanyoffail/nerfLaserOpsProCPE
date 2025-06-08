#include <Adafruit_CircuitPlayground.h>
#include <Adafruit_Circuit_Playground.h>

#define myReceiver CircuitPlayground.irReceiver
#define myDecoder CircuitPlayground.irDecoder

/* IR hashes for Nerf LaserOps Pro standard game */
#define alphaPointPurple 0x67228B44
#define alphaPointRed 0x78653B0E
#define alphaPointBlue 0x2FFEA610

#define deltaBurstPurple 0xD303E9B8
#define deltaBurstRed 0xE4469982
#define deltaBurstBlue 0x9BE00484

#define PURPLE 0x00FF00FF
#define RED 0x00FF0000
#define GREEN 0x0000FF00
#define BLUE 0x000000FF

Adafruit_CPlay_NeoPixel strip = Adafruit_CPlay_NeoPixel(10, CPLAY_NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

int ANIMATION_DELAY = 200;
int HOLD_DELAY = 350;

// Purple Team = 0
// Red Team = 1
// Blue Team = 2
int ownerTeam = 0;

// Hit Points and Damage
int hitPointMax = 10;
int hitPoint = hitPointMax;

// Hits will only be valid from opposing team if Red or Blue, or any Purple
// Prevents Red/Blue from affecting Purple and vice versa to copy stock behaviour.
// Maybe there is a game mode that can be tied to modyfing this behaviour somehow?
bool validHit() {
  if(myDecoder.protocolNum==UNKNOWN) {
    switch (ownerTeam) {
      case 0:
        if (myDecoder.value == alphaPointPurple || myDecoder.value == deltaBurstPurple) {
          return true;
        }
        break;
      case 1:
        if (myDecoder.value == alphaPointBlue || myDecoder.value == deltaBurstBlue) {
          return true;
        }
        break;
      case 2:
        if (myDecoder.value == alphaPointRed || myDecoder.value == deltaBurstRed) {
          return true;
        }
        break;
    }
    return false;
  }
}

void validHitResponse() {
  ANIMATION_DELAY = 0; //ms
  HOLD_DELAY = 250; //ms
  --hitPoint;
  switch (ownerTeam) {
    case 0:
      Serial.print("VALID HIT PURPLE 0x");
      Serial.println(myDecoder.value, HEX);
      setLEDColor(PURPLE);
      break;
    case 1:
      Serial.print("VALID HIT BLUE 0x");
      Serial.println(myDecoder.value, HEX);
      setLEDColor(BLUE);
      break;
    case 2:
      Serial.print("VALID HIT RED 0x");
      Serial.println(myDecoder.value, HEX);
      setLEDColor(RED);
      break;
  }
  delay(HOLD_DELAY);
  setLEDNone();
}

// Cycle ownerTeam variable between teams and show a boot-up animation to indicate ownerTeam color when Right Button is pressed.
void teamSwitchButton() {
  ANIMATION_DELAY = 180;
  HOLD_DELAY = 500;
  setLEDNone();
  if (ownerTeam < 2) {
    ++ownerTeam;
  }
  else {
    ownerTeam = 0;
  }
  switch (ownerTeam) {
    case 0:
      setLEDColor(PURPLE);
      break;
    case 1:
      setLEDColor(RED);
      break;
    case 2:
      setLEDColor(BLUE);
      break;
  }
  delay(HOLD_DELAY);
  hitPointLEDGreen();
}

// Reset the unit to full health on current ownerTeam when Left Button is pressed.
void resetButton() {
  hitPoint = hitPointMax;
  setLEDNone();
  switch (ownerTeam) {
    case 0:
      setLEDColor(PURPLE);
      break;
    case 1:
      setLEDColor(RED);
      break;
    case 2:
      setLEDColor(BLUE);
      break;
  }
  hitPointLEDGreen();
}

// set all NeoPixels to the given color
void setLEDColor(uint32_t color)
{
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;

  for (int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, r, g, b);
    strip.show();
    delay(ANIMATION_DELAY);
  }
}

void hitPointLEDGreen() {
  setLEDNone();
  for (int i=0; i<hitPoint; i++) {
    strip.setPixelColor(i, 0x00, 0xFF, 0x00);
    strip.show();
    delay(ANIMATION_DELAY);
  }
}

void setLEDNone() {
  strip.clear();
  strip.show();
}

void noHitPoint() {
  ANIMATION_DELAY = 1000;
//  while (hitPoint == 0) {
    for (int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 0xFF, 0xFF, 0xFF);
      strip.show();
      delay(ANIMATION_DELAY);
    }
    setLEDNone();
    if (CircuitPlayground.rightButton()) {
      resetButton();
    }
//  }
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
  setLEDColor(PURPLE);
  setLEDColor(GREEN);
}

void loop() {
    // Activate IR Receiver
  if (myReceiver.getResults()) {
    myDecoder.decode();
    if (validHit() == true && hitPoint >= 2) {
      validHitResponse();
      hitPointLEDGreen();
    }
    else if (validHit() == true && hitPoint == 1) {
      validHitResponse();
      noHitPoint();
    }
    else {
      Serial.print("INVALID HIT 0x");
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
