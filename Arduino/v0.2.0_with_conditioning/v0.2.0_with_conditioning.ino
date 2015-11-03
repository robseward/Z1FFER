#include "AES.h"
#include "CBC.h"

#define CHAIN_SIZE 2
#define BLOCK_SIZE 16
#define SAMPLE_SIZE (BLOCK_SIZE * CHAIN_SIZE)

byte key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

CBC<AES128> cbc;


byte sourcePool[SAMPLE_SIZE];
 
void setup()
{
  Serial.begin(115200);
  //Set B pins to input
  DDRB = B00000000;
  delay(1000);
  
} 


void loop() {
  static byte currentByte = 0x00; 
  byte pinVal = (PINB >> 3) & B00000001;
  boolean byteReady = collectBit(pinVal, &currentByte);
 
  if (byteReady){
    //Serial.println("ByteCollected");
    boolean poolFull = collectByte(currentByte);
    if (poolFull){
      conditionPool();
      //Serial.write(sourcePool, SAMPLE_SIZE);
    }
    currentByte = 0;
  }
}

//@return true if byte is full/complete
boolean collectBit(byte inputBit, byte *currentByte){
  static int bitCounter = 0;
  *currentByte |= inputBit << bitCounter;
   bitCounter = ++bitCounter % 8;
  
  if (bitCounter == 0) {
    return true;
  }
  return false;
}

//@return true if sourcePool is full
boolean collectByte(byte currentByte){
  static int byteCount = 0;
  sourcePool[byteCount] = currentByte;
  byteCount = ++byteCount % SAMPLE_SIZE;
  //Serial.println(byteCount);

  if (byteCount == 0){
    return true;
  }
  return false;
}
 
 //TODO: this should return encrypted pool, and mac should
 //be sent in the calling function
void conditionPool(){
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

