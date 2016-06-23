#include "AES.h"
#include "CBC.h"

#define CHAIN_SIZE 2
#define BLOCK_SIZE 16  // this is defined in AES library. We should probably set it dynamically.
#define SAMPLE_SIZE (BLOCK_SIZE * CHAIN_SIZE)

byte key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

CBC<AES128> cbc;

byte sourcePool[SAMPLE_SIZE];
byte moduloReplacement;

bool flip = false;

int currentByte = 0;
int bitCounter=0;
 
/******* SETUP *********/
 
void setup()
{
  // Defining PB1 and PB2 as outputs by setting PORTB1 and PORTB2
  // Setting DDB1 and DDB2
  DDRB |= bit (DDB1) | bit (DDB2);

  //Set D pins to input
  DDRD = 0x00;

  Serial.begin(2000000);
  
  calculateModuloReplacement();
} 


//This method is so we don't have to use modulo in collectBytes().
//(Modulo is very slow on Arduino.)
void calculateModuloReplacement(){
  double logValue = log(SAMPLE_SIZE) / log(2);
  double roundedLogValue = round(logValue);
  
  //Assert that sample size is a power of 2
  //Otherwise our fast modulo technique will not work
  if (fabs(logValue - roundedLogValue) > 0.001) {
    abort();
  }
  
  int variableSizeInBits = sizeof(moduloReplacement) * 8;
  moduloReplacement = 0xFF >> (variableSizeInBits - (byte)roundedLogValue);
}


void loop() {
  currentByte = 0;
  for (bitCounter=0; bitCounter < 8; bitCounter += 2) {
    runClockSignals();
    runClockSignals();
    byte pinVal = (PIND >> 6) & B00000011;
    currentByte |= pinVal << bitCounter;
  }
  boolean poolFull = collectByte(currentByte);
    if (poolFull){
      conditionPoolAndWriteToSerial();
      //Serial.write(sourcePool, SAMPLE_SIZE);
    }
}

void runClockSignals(){
  if (flip) {
    PORTB = 0b00000010;
  }
  else {
    PORTB = 0b00000100;
  }
  flip = !flip;
}


//@return true if sourcePool is full
boolean collectByte(byte currentByte){
  static int byteCount = 0;
  sourcePool[byteCount] = currentByte;
  byteCount = (++byteCount) & moduloReplacement;
  //Serial.println(byteCount);

  if (byteCount == 0){
    return true;
  }
  return false;
}
 
 //TODO: this should return encrypted pool, and mac should
 //be sent in the calling function
void conditionPoolAndWriteToSerial(){
  //Serial.println("Conditioning");
  static byte iv[BLOCK_SIZE] = {0};
  byte output[SAMPLE_SIZE];
  cbc.clear();
  cbc.setKey(key, 16);
  cbc.setIV(iv, 16);
  cbc.encrypt(output, sourcePool, SAMPLE_SIZE);
  
  //advance pointer to last block (the MAC)
  byte *outBuf = &output[SAMPLE_SIZE - BLOCK_SIZE]; 
  Serial.write(outBuf, BLOCK_SIZE);
}

