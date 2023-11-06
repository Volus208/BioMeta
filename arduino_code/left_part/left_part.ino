#include <Adafruit_Fingerprint.h>
#include <Adafruit_NeoPixel.h>
#include <Keyboard.h>
#include "password.h"

// Settings
#define ROLLOVER 5
#define COM_SPEED 100 //speed of the communication between the two halves in Hz
#define LAYOUT KeyboardLayout_fr_FR
#define FADESPEED 15
#define HIGHESTMODE 4 //substract one to the actual number of modes, since mode 0 exists

#define COM_DELAY 16129/COM_SPEED
#define SPECIALKEYS 30
#define MESSAGE_SIZE ROLLOVER*6+1
#define LED_COUNT 98
#define LED_PIN 20
#define ROWS 6
#define COLS 6
#define RXPIN 2
#define CLOCKPIN 3
#define mySerial Serial1

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

byte receivedKeyNumber;
bool coordinatesFound;
const byte rowPins[ROWS] = {19, 18, 15, 14, 16, 10};
const byte colPins[COLS] = {4, 5, 6, 7, 8, 9};
const char characters[98] = {177, 194, 195, 196, 197, 198, 136+0x35, '&', 136+0x1f, 34, 39, '(', 179, 'a', 'z', 'e', 'r', 't', 193, 'q', 's', 'd', 'f', 'g', 129, 'w', 'x', 'c', 'v', 'b', 128, 131, 130, ' ', //left part
214, 211, 212, 209, 205, 204, 203, 202, 201, 200, 199, '-', 136+0x24, '_', 136+0x26, 136+0x27, ')', '=', 178, 219, 220, 221, 222, 233, 232, 231, '<', '$', 136+0x2f, 'p', 'o', 'i', 'u', 'y', 'h', 'j', 'k', 'l', 'm', 136+0x34, 176, 228, 229, 230, 223, 227, 226, 225, 218, 133, '!', ':', ';', ',', 'n', ' ', 134, 0, 216, 217, 215, 234, 235, 224}; //right part
const byte specialKeys[30] = {13, 19, 25, 14, 20, 26, 15, 21, 27, 16, 22, 28, 17, 23, 29, 67, 68, 88, 66, 69, 87, 65, 70, 86, 64, 71, 85, 63, 72, 84};
const byte coordinates[6][18] = { { 0,  1,  2,  3,  4,  5, 44, 43, 42, 41, 40, 39, 38, 98, 37, 36, 35, 34},
                                  { 6,  7,  8,  9, 10, 11, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56},
                                  {12, 13, 14, 15, 16, 17, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 98},
                                  {18, 19, 20, 21, 22, 23, 68, 69, 70, 71, 72, 73, 98, 74, 75, 76, 77, 78},
                                  {24, 25, 26, 27, 28, 29, 88, 87, 86, 85, 84, 83, 98, 82, 81, 80, 79, 98},
                                  {30, 31, 32, 98, 33, 98, 98, 98, 89, 98, 90, 91, 92, 93, 94, 95, 96, 97} };
bool keyValues[98];
bool previousKeyValues[98];
bool receivedMessage[MESSAGE_SIZE];
byte mode = 0;
byte iteration = 0;
bool fingerprintEnabled = 0;
byte redValue;
byte greenValue;
byte blueValue;
uint32_t pixelColor;
unsigned int hue = 0;

void setup() {
  pinMode(RXPIN, INPUT_PULLUP);
  pinMode(CLOCKPIN, OUTPUT);
	delay(1000);
  Keyboard.begin(LAYOUT);
  Keyboard.releaseAll();
  strip.begin();
}

void loop() {
  for (byte k=0; k<98; k++){ //update the list of keys that were pressed last time they got scanned
    previousKeyValues[k] = keyValues[k];
  }
  scanKeys();
  for(byte messageBitNumber=0; messageBitNumber<MESSAGE_SIZE; messageBitNumber++){ //request the key values from the right part
    digitalWrite(CLOCKPIN, LOW);
    delayMicroseconds(COM_DELAY);
    receivedMessage[messageBitNumber] = digitalRead(RXPIN);
    digitalWrite(CLOCKPIN, HIGH);
    delayMicroseconds(COM_DELAY);
  }
  binaryToInt();
  updateLeds();
  updateSpecialKey();

  for (byte k=0; k<98; k++){ //send keys to USB
    if(previousKeyValues[k] != keyValues[k]){ //only if the key value has changed
      if(keyValues[k]){
        Keyboard.press(characters[k]);
      }else{
        Keyboard.release(characters[k]);
      }
    }
  }

  if(fingerprintEnabled){
    if(getFingerprintIDez() != -1){ //if fingerprint is valid
      Keyboard.print(PASSWORD);
      Keyboard.write(10);
      fingerprintEnabled = 0;
      finger.end();
    }
  }
  strip.show();
  iteration += 1;
}

void scanKeys(){
  for (byte i=0; i<COLS; i++){
    pinMode(colPins[i], INPUT_PULLUP);
  }
  for (byte r=0; r<ROWS; r++){
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);
    for (byte c=0; c<COLS; c++){
      keyValues[r*6+c] = !digitalRead(colPins[c]);
    }
    digitalWrite(rowPins[r], HIGH);
    pinMode(rowPins[r], INPUT);

  }
}

void binaryToInt(){
  for(byte i=0; i<64; i++){
    keyValues[34+i] = 0;
  }
  if(receivedMessage[MESSAGE_SIZE-1]){ //if the last bit is a 1, the first key is pressed, so only check the 4 other keys
    keyValues[34] = 1;
    for(byte keyNumber=1; keyNumber<ROLLOVER; keyNumber++){
      receivedKeyNumber = (receivedMessage[(keyNumber*6)]+receivedMessage[(keyNumber*6)+1]*2+receivedMessage[(keyNumber*6)+2]*4+receivedMessage[(keyNumber*6)+3]*8+receivedMessage[(keyNumber*6)+4]*16+receivedMessage[(keyNumber*6)+5]*32)+1;
      if(receivedKeyNumber != 1){
        keyValues[receivedKeyNumber+33] = 1;
      }
   }
  } else { //if the last bit is not a 1
  keyValues[34] = 0;
    for(byte keyNumber=0; keyNumber<ROLLOVER; keyNumber++){
      receivedKeyNumber = (receivedMessage[(keyNumber*6)]+receivedMessage[(keyNumber*6)+1]*2+receivedMessage[(keyNumber*6)+2]*4+receivedMessage[(keyNumber*6)+3]*8+receivedMessage[(keyNumber*6)+4]*16+receivedMessage[(keyNumber*6)+5]*32)+1;
      if(receivedKeyNumber != 1){
        keyValues[receivedKeyNumber+33] = 1;
      }
    }
  }
  
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  return finger.fingerID; 
}

void updateLeds(){
  hue = hue + 160;
  for (byte k=0; k<98; k++){ //update the LED values without sending them
    if(keyValues[k] == 1){
      strip.setPixelColor(k, strip.ColorHSV(hue, 255, 255)); // if the k key is pressed, light up it's led
      if(mode == 4){ //if mode 4 is on, search for the key's coordinates and light up the leds around it
      coordinatesFound = false;
      for(byte y=0; y<6; y++){ 
        for(byte x=0; x<18; x++){
          if(coordinates[y][x] == k){
            if(x != 0){
              strip.setPixelColor(coordinates[y][x-1], strip.ColorHSV(hue, 255, 255));
            }
            if(x != 17){
              strip.setPixelColor(coordinates[y][x+1], strip.ColorHSV(hue, 255, 255));
            }
            if(y != 0){
              strip.setPixelColor(coordinates[y-1][x], strip.ColorHSV(hue, 255, 255));
            }
            if(y != 5){
              strip.setPixelColor(coordinates[y+1][x], strip.ColorHSV(hue, 255, 255));
            }
            coordinatesFound = true;
            break;
          }
        }
        if(coordinatesFound){
            break;
          }
      }
    }
    } else {
      pixelColor = strip.getPixelColor(k);
      if(pixelColor > 0){
        redValue = Red(pixelColor);
        greenValue = Green(pixelColor);
        blueValue = Blue(pixelColor);
        if(redValue > 0){
          if(redValue >= FADESPEED){
            redValue = redValue - FADESPEED;
          } else {
            redValue = 0;
          }
        }
        if(greenValue > 0){
          if(greenValue >= FADESPEED){
            greenValue = greenValue - FADESPEED;
          } else {
            greenValue = 0;
          }
        }
        if(blueValue > 0){
          if(blueValue >= FADESPEED){
            blueValue = blueValue - FADESPEED;
          } else {
            blueValue = 0;
          }
        }
        strip.setPixelColor(k, redValue, greenValue, blueValue);
      }
    }
  }
  if (mode == 1){
    for(byte i=0; i<SPECIALKEYS; i++){ //rainbow on the letters
      strip.setPixelColor(specialKeys[i], strip.ColorHSV(hue+(1500*i), 255, 128));
    }
  } else if (mode == 2){
    if((iteration % 10) == 0){ //sparkling effect
      strip.setPixelColor(random(0, 98), strip.ColorHSV(hue, 255, 255));
    }
  } else if (mode == 3){ //light following the LED wiring order
    strip.setPixelColor(map(iteration, 0, 255, 0, 97), strip.ColorHSV(hue, 255, 255));
  }
  if(fingerprintEnabled){ //turn on the RGB key if the fingerprint sensor is enabled
    strip.setPixelColor(91, 0, 255, 0);
  }
}

void updateSpecialKey(){
  if(keyValues[91] && previousKeyValues[91] == 0){
    if(keyValues[90] == 0){ //change the mode
      if(mode == HIGHESTMODE){
        mode = 0;
      } else {
        mode += 1;
      }
      if(mode == 1){
        for(byte i=0; i<SPECIALKEYS; i++){
        strip.setPixelColor(specialKeys[i], strip.ColorHSV(hue+(1500*i), 255, 128));
        }
      }
      
    } else{
      fingerprintEnabled = 1;
      finger.begin(57600);
      delay(10);
    }
  }
}

// Returns the Red component of a 32-bit color
uint8_t Red(uint32_t color)
{
    return (color >> 16) & 0xFF;
}
// Returns the Green component of a 32-bit color
uint8_t Green(uint32_t color)
{
    return (color >> 8) & 0xFF;
}
 // Returns the Blue component of a 32-bit color
uint8_t Blue(uint32_t color)
{
    return color & 0xFF;
}
 
