#include <Adafruit_Circuit_Playground.h>

//Less typing is nice.
#define myReceiver CircuitPlayground.irReceiver
#define myDecoder CircuitPlayground.irDecoder
#define strip CircuitPlayground.strip

// IR hashes for Nerf LaserOps Pro standard game: AP AlphaPoint/DB DeltaBurst
#define AP_PURPLE 0x67228B44
#define AP_RED 0x78653B0E
#define AP_BLUE 0x2FFEA610

#define DB_PURPLE 0xD303E9B8
#define DB_RED 0xE4469982
#define DB_BLUE 0x9BE00484

const uint32_t alphaPoint[] = {AP_PURPLE, AP_RED, AP_BLUE};
const uint32_t deltaBurst[] = {DB_PURPLE, DB_RED, AP_BLUE};

//Color codes for LED strip
#define PURPLE 0xFF00FF
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define YELLOW 0xFFFF00
#define WHITE 0xFFFFFF

const int animationPixelR[] = {0, 1, 2, 3, 4};
const int animationPixelL[] = {9, 8, 7, 6, 5};

// Purple Team = 0
// Red Team = 1
// Blue Team = 2
int teamOwner = 0;
int teamEnemy = 0;
const uint32_t teamColor[] = {PURPLE, RED, BLUE};
const char* teamName[] = {"PURPLE", "RED", "BLUE"};

// Hit Point settings
#define HIT_POINT_BASE 10
int HIT_MULTIPLIER = 1;
int hitPoint = HIT_POINT_BASE * HIT_MULTIPLIER;

// Hits will only be valid from opposing team if Red or Blue, or any Purple
// Prevents Red/Blue from affecting Purple and vice versa to copy stock behaviour.
bool validHit() {
  myDecoder.decode();

  return myDecoder.protocolNum == UNKNOWN && 
    (myDecoder.value == alphaPoint[teamEnemy] ||
    myDecoder.value == deltaBurst[teamEnemy]) &&
    hitPoint > 0;
}

//Do things when hit with a valid blaster.
void validHitResponse() {
  --hitPoint;
  Serial.print(F("VALID HIT "));
  Serial.print(teamName[teamEnemy]);
  Serial.print(F(" 0x"));
  Serial.println(myDecoder.value, HEX);
  CircuitPlayground.clearPixels();
  strip.fill(teamColor[teamEnemy], 0, strip.numPixels());
  strip.show();
  delay(240);
  showHitPoint();
}

// Cycle teamOwner variable between teams and show a boot-up animation to indicate teamOwner color when Right Button is pressed.
void teamSwitchButton() {
  HIT_MULTIPLIER = 1;
  hitPoint = HIT_POINT_BASE * HIT_MULTIPLIER;
  teamOwner = ++teamOwner % 3;
  teamEnemy = (--teamEnemy + 3) % 3;
  CircuitPlayground.clearPixels();
  setLED(teamColor[teamOwner], 200);
  showHitPoint();
}

// Cycle between 10/20/30 Hit Points.
void modeButton() {
  CircuitPlayground.clearPixels();
  HIT_MULTIPLIER = HIT_MULTIPLIER++ % 3 +1;
  hitPoint = HIT_POINT_BASE * HIT_MULTIPLIER;
  strip.fill(WHITE, 1, HIT_MULTIPLIER);
  strip.show();
  delay(420);
  showHitPoint();
}

//Show hit points on Neopixel LED
void showHitPoint() {
  CircuitPlayground.clearPixels();
  strip.fill(GREEN, 0, hitPoint);
  strip.show();
}

//Set all Neopixel LED, with optional animation delay; escape animations with the buttons.
void setLED (uint32_t color, int animation_delay) {
 CircuitPlayground.clearPixels();
  for (int i=0; i<strip.numPixels(); i++) { 
    strip.setPixelColor(animationPixelR[i], color);
    strip.setPixelColor(animationPixelL[i], color);
    strip.show();
    delay(animation_delay);
    if (CircuitPlayground.rightButton()) {
      teamSwitchButton();
      return;
    }
    if (CircuitPlayground.rightButton()) {
      modeButton();
      return;
    }
  }
}

void setup() {
  CircuitPlayground.begin();
  Serial.begin(9600);
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals"));
  strip.setBrightness(12);
  CircuitPlayground.clearPixels();
  setLED(teamColor[teamOwner], 200);
  showHitPoint();
}

void loop() {
  //Loop death animation when hitPoints is 0; you can hold the teamSwitchButton/modeButton to reset HP and escape the loop.
  if (hitPoint <= 0) {
    setLED(YELLOW, 420);
  }

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
