/* 
 *  Z1FFER
 *  OpenRandom.org
 *  
 *  2015 Rob Seward
 *  
 *  Sends raw random data to the host as fast as possible. 
 *  Baud rate is 230400 
 * 
 */
 
void setup()
{
  Serial.begin(230400);
  //Set B pins to input
  DDRB = B00000000;
  delay(1000);
} 

void loop() {
  static byte currentByte = 0x00; 
  byte pinVal = (PINB >> 3) & B00000001;
  boolean byteReady = collectBit(pinVal, &currentByte);
 
  if (byteReady){
    Serial.write(currentByte);
    currentByte = 0;
  }
}

//@return true if byte is full/complete
boolean collectBit(byte inputBit, byte *currentByte){
  static int bitCounter = 0;
  *currentByte |= inputBit << bitCounter;
   bitCounter = ++bitCounter & 0b00000111;
  
  if (bitCounter == 0) {
    return true;
  }
  return false;
}

