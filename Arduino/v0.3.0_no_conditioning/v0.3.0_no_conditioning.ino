
/**
 * Z1FFER v0.3.X Firmware.
 *
 * This code is designed to be as fast as possible.
 * It operates two inverted clock signals on pins 9 and 10
 * while reading in random bytes on pins 6 and 7.
 * Pins are read and written to using the registers for speed.
 * The clock signal is irregular but this does not effect the
 * performance of the Z1FFER shield. 
 */

bool flip = false;
int currentByte = 0;
int i=0;

void setup() {
  //Set B pins to output
  DDRB = 0b11111111;
  //Set D pins to input
  DDRD = 0x00;

  //2,000,000 baud is the theoretical maximum baud of Arduino
  Serial.begin(2000000);
}

void loop() {

  //Collect a byte from the two bitstream pins on the Z1FFER board
  currentByte = 0;
  for (i=0; i < 8; i += 2) {
    runClockSignals();
    runClockSignals();

    //Use the pin register to read pin values instead of pinRead() because it is much faster
    byte pinVal = (PIND >> 6) & B00000011;
    currentByte |= pinVal << i;
  }
  Serial.write(currentByte);
}

//Oscillate a clock signal and its inverse on the clock pins
//Note: the signals do not need to be percisely timed.
void runClockSignals(){
  if (flip) {
    PORTB = 0b00000010;
  }
  else {
    PORTB = 0b00000100;
  }
  flip = !flip;
}
