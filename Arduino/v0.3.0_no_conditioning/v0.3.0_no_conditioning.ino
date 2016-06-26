
bool flip = false;
 
void setup() {
  //Set B pins to output
  DDRB = 0b11111111;
  //Set D pins to input
  DDRD = 0x00;
  
  Serial.begin(2000000);
}

int currentByte = 0;
int i=0;

void loop() {
  currentByte = 0;
  for (i=0; i < 8; i += 2) {
    runClockSignals();
    runClockSignals();
    byte pinVal = (PIND >> 6) & B00000011;
    currentByte |= pinVal << i;
  }
  Serial.write(currentByte);
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


