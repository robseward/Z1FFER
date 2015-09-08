#include "AES.h"

#define COLLECTING 0
#define CONDITIONING 1

AES128 aes128;
byte key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};


 
void setup()
{
  // Defining PB1 and PB2 as outputs by setting PORTB1 and PORTB2
  // Setting DDB1 and DDB2
  DDRB |= bit (DDB1) | bit (DDB2);

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
  ICR1 = 45;
  OCR1B = 22;
  OCR1A = 22;

  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();

  Serial.begin(230400);

  //Set D pins to input
  DDRD = 0x00;
  
  aes128.setKey(key, aes128.keySize());
} 

byte currentByte = 0x00;
int bitCounter = 0;
bool flip = false;
ISR(TIMER1_COMPA_vect) { 
  byte pinInput = 0;
  pinInput = PIND >> 7;
  currentByte |= pinInput << bitCounter;

  if (bitCounter == 7) {
    //Serial.write(currentByte);
    currentByte = 0;
  }

  bitCounter = ++bitCounter % 8;
}

void processByte(byte currentByte){


}

void loop()
{
}
