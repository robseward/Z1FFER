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
 
/******* SETUP *********/
 
void setup()
{
  
  //TODO: move interrupt configuration into its own function
  //Maybe call it configureClockSignals()
  
  // Defining PB1 and PB2 as outputs by setting PORTB1 and PORTB2
  // Setting DDB1 and DDB2
  DDRB |= bit (DDB1) | bit (DDB2);

  //Set D pins to input
  DDRD = 0x00;

  Serial.begin(2000000);
  
  calculateModuloReplacement();
  setupClockSignals();
  enableInterrupts();
} 

void setupClockSignals() {
    // stop timer 1
  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1A = bit (COM1B0) | bit (COM1B1)  // Set OC1B on Compare Match, clear
           // OC1B at BOTTOM (inverting mode)
           | bit (COM1A1)                 // Clear OC1A on Compare Match, set
           // OC1A at BOTTOM (non-inverting mode)
           | bit (WGM11) | bit (WGM13);                 // Fast PWM, top at ICR1
  TCCR1B = bit (WGM12)  | bit (WGM13)   //       ditto
           | bit (CS11);                  // Start timer, prescaler of 8

  // Initialize OCR1A = 300 (pulse_width = 150us), OCR1B, and ICR1
  ICR1 = 15;
  OCR1B = 7;
  OCR1A = 7;
//   ICR1 = 45;
//  OCR1B = 22;
//  OCR1A = 22;
}

void enableInterrupts(){
    // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();
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

/******** Interrupt Callback and helpers ********/

ISR(TIMER1_COMPA_vect) {
  static byte currentByte = 0x00; 
    byte pinVals = (PIND >> 6) & 0b00000011;
  boolean byteReady = collectBits(pinVals, &currentByte);

 
  if (byteReady){
//    Serial.write(currentByte);
    boolean poolFull = collectByte(currentByte);
    if (poolFull){
      conditionPoolAndWriteToSerial();
//      Serial.write(sourcePool, SAMPLE_SIZE);
    }
    currentByte = 0;
  }
}

//@return true if byte is full/complete
boolean collectBits(byte inputBits, byte *currentByte){
  static int bitCounter = 0;
  *currentByte |= inputBits << bitCounter;
  bitCounter += 2;

  //modulo is very slow, so we do a bitwise operation. Equivelant to % 8
  bitCounter = bitCounter & 0b00000111; 
  
  if (bitCounter == 0) {
    return true;
  }
  return false;
}

//@return true if sourcePool is full
boolean collectByte(byte currentByte){
  static int byteCount = 0;
  sourcePool[byteCount] = currentByte;
  byteCount = (++byteCount) & moduloReplacement;

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

void loop()
{
}
