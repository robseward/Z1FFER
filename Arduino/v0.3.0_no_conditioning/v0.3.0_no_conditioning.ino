
byte moduloReplacement;
 
/******* SETUP *********/
 
void setup()
{
  // Defining PB1 and PB2 as outputs by setting PORTB1 and PORTB2
  // Setting DDB1 and DDB2
  DDRB |= bit (DDB1) | bit (DDB2);

  //Set D pins to input
  DDRD = 0x00;

  Serial.begin(230400);
  //Serial.println("Starting...");
  
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

/******** Interrupt Callback and helpers ********/

ISR(TIMER1_COMPA_vect) {
  static byte currentByte = 0x00; 
  byte pinVal = PIND >> 7;
  boolean byteReady = collectBit(pinVal, &currentByte);
 
//  PORTD = 0b00000100;
//  delayMicroseconds(1);
//  PORTD = 0b00000000;
 
  if (byteReady){
    Serial.write(currentByte);
    currentByte = 0;
  }
}

//@return true if byte is full/complete
boolean collectBit(byte inputBit, byte *currentByte){
  static int bitCounter = 0;
  *currentByte |= inputBit << bitCounter;
  //modulo is very slow, so we do a bitwise operation. Equivelant to % 8
   bitCounter = (++bitCounter) & 0b00000111; 
  
  if (bitCounter == 0) {
    return true;
  }
  return false;
}

void loop()
{
}
