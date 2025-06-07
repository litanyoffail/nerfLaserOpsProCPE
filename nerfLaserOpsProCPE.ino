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

Adafruit_CPlay_NeoPixel strip = Adafruit_CPlay_NeoPixel(10, CPLAY_NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

static const unsigned long ANIMATION_DELAY = 150; //ms
static const unsigned long HOLD_DELAY = 300; //ms

// static const long purpleTeam[2] = {alphaPointPurple, deltaBurstPurple};
// static const long redTeam[2] = {alphaPointRed, deltaBurstRed};
// static const long blueTeam[2] = {alphaPointBlue, deltaBurstBlue};

// Purple Team = 0
// Red Team = 1
// Blue Team = 2
int ownerTeam = 1;

// Hits will only be valid from opposing team if Red or Blue, or any Purple
// Prevents Red/Blue from affecting Purple and vice versa to copy stock behaviour.
// Maybe there is a game mode that can be tied to modyfing this behaviour somehow?
bool validHit() {
  if(myDecoder.protocolNum==UNKNOWN) {
    if (ownerTeam == 0) {
      if (myDecoder.value == alphaPointPurple) {
        return true;
      }
      else if (myDecoder.value == deltaBurstPurple) {
        return true;
      }
    }
    else if (ownerTeam == 1) {
      if (myDecoder.value == alphaPointBlue) {
        return true;
      }
      else if (myDecoder.value == deltaBurstBlue) {
        return true;
      }
    }
    else if (ownerTeam == 2) {
      if (myDecoder.value == alphaPointRed) {
        return true;
      }
      else if (myDecoder.value== deltaBurstRed) {
        return true;
      }
    }
    return false;
  }
}

void validHitResponse() {
  Serial.print("VALID HIT 0x");
  Serial.println(myDecoder.value, HEX);
  setLEDPurple ();
  delay(HOLD_DELAY);
  strip.clear();
  strip.show();
}

// Cycle ownerTeam variable between teams and show a boot-up animation to indicate ownerTeam color when Right Button is pressed.
//void teamSwitchButton() {
//  if (ownerTeam == 0) {
//    for (int i=0; i<strip.numPixels(); i++) {
//      strip.setPixelColor(i, 0xFF, 0x00, 0xFF);
//      strip.show();
//      delay(ANIMATION_DELAY);
//    }
//    delay(HOLD_DELAY);
//    strip.clear();
//    strip.show();
//    delay(HOLD_DELAY);
//  }
//  else if (ownerTeam == 1) {
//    for (int i=0; i<strip.numPixels(); i++) {
//      strip.setPixelColor(i, 0xFF, 0x00, 0x00);
//      strip.show();
//      delay(ANIMATION_DELAY);
//    }
//    delay(HOLD_DELAY);
//    strip.clear();
//    strip.show();
//    delay(HOLD_DELAY);
//  }
//  else if (ownerTeam == 2) {
//    for (int i=0; i<strip.numPixels(); i++) {
//      strip.setPixelColor(i, 0x00, 0x00, 0xFF);
//      strip.show();
//      delay(ANIMATION_DELAY);
//    }
//    delay(HOLD_DELAY);
//    strip.clear();
//    strip.show();
//    delay(HOLD_DELAY);
//  }
//}

// Reset the unit to full health on current ownerTeam when Left Button is pressed.
//void resetButton() {
//}

// Set All NeoPixels to Purple
void setLEDPurple () {
  for (int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, 0xFF, 0x00, 0xFF);
  }
  strip.show();
}

// Set All NeoPixels to Red
void setLEDRed () {
  for (int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, 0xFF, 0x00, 0x00);
  }
  strip.show();
}

// Set All NeoPixels to Blue
void setLEDBlue () {
  for (int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, 0x00, 0x00, 0xFF);
  }
  strip.show();
}

void setup() {
  CircuitPlayground.begin();
  Serial.begin(9600);
//  while (!Serial); //delay to wait for serial port
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals"));
  strip.begin();
  strip.setBrightness(64);
  strip.clear();
  strip.show();
}

void loop() {
    // Activate IR Receiver
  if(myReceiver.getResults()) {
    myDecoder.decode();
    if(validHit() == true) {
      validHitResponse ();
    }
    myReceiver.enableIRIn();
  }
  
}
