// Settings
#define ROWS 8
#define COLS 8
#define ROLLOVER 5

#define MESSAGE_SIZE ROLLOVER*6+1
#define TXPIN 2
#define CLOCKPIN 3
const byte rowPins[ROWS] = {4, 5, 6, 7, 8, 9, 10, 16};
const byte colPins[COLS] = {14, 15, 18, 19, 20, 21, 1, 0};
const byte keyNumbers[64] = {11, 9, 7, 6, 5, 4, 3, 1, 10, 8, 16, 18, 19, 20, 2, 23, 12, 14, 17, 29, 28, 26, 21, 24, 13, 15, 31, 40, 27, 42, 22, 45, 34, 33, 30, 39, 41, 48, 25, 44, 35, 32, 37, 52, 49, 59, 43, 46, 55, 36, 38, 51, 50, 60, 47, 64, 56, 54, 53, 57, 58, 61, 62, 63};

byte pressedKeys[ROLLOVER];
byte keysAlreadyPressed = 0;
bool messageToSend[MESSAGE_SIZE];

void setup() {
  pinMode(TXPIN, OUTPUT);
  pinMode(CLOCKPIN, INPUT_PULLUP);
}


void loop() {
  scanKeys();

  for(byte pressedKeyNumber=0; pressedKeyNumber<ROLLOVER; pressedKeyNumber++){ //prepare the binary message to send to the other arduino
    if(pressedKeys[pressedKeyNumber] != 0){
      for(byte bitNumber=0; bitNumber<6; bitNumber++){
        messageToSend[(pressedKeyNumber*6)+bitNumber] = bitRead(pressedKeys[pressedKeyNumber]-1, bitNumber);
      }
    } else {
      for(byte bitNumber=0; bitNumber<6; bitNumber++){
        messageToSend[(pressedKeyNumber*6)+bitNumber] = 0;
      }
    }
  }
  if(pressedKeys[0] == 1){ //set the last bit to differenciate 0 (no keys) and 0th key pressed
    messageToSend[MESSAGE_SIZE-1] = 1;
  } else {
    messageToSend[MESSAGE_SIZE-1] = 0;
  }

  for(byte messageBitNumber=0; messageBitNumber<MESSAGE_SIZE; messageBitNumber++){
    digitalWrite(TXPIN, messageToSend[messageBitNumber]); //send a bit
    while(digitalRead(CLOCKPIN)){
      //wait for the clock pin to be pulled low when the other arduino is ready to receive a bit
    }
    while(digitalRead(CLOCKPIN) == 0){
      //wait for the clock pin to be high when the other arduino registers the bit
    }
  }

  for (byte b=0; b<ROLLOVER; b++){ //reset the list of pressed keys
    pressedKeys[b] = 0;
  }
}

void scanKeys(){
  for (byte i=0; i<COLS; i++){
    pinMode(colPins[i], INPUT_PULLUP);
  }
  for (byte r=0; r<ROWS; r++){
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);
    for (byte c=0; c<COLS; c++){
      if (!digitalRead(colPins[c])){
        if (keysAlreadyPressed < ROLLOVER){
          pressedKeys[keysAlreadyPressed] = keyNumbers[((r*COLS)+c)];
          keysAlreadyPressed += 1;
        }
      }
    }
    digitalWrite(rowPins[r], HIGH);
    pinMode(rowPins[r], INPUT);
  }
  keysAlreadyPressed = 0;

}