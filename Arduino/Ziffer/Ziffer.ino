/*****************************
 * Z1FFER
 * OpenRandom.org
 *
 * Created by Rob Seward 2013-2014
 * 2013-2014 VHS Design LLC
/******************************/
 
byte previousByte = 0;
byte previousMixedByte1 = 0;
byte previousMixedByte2 = 0;
byte previousMixedByte3 = 0;
byte previousMixedByte4 = 0;

void setup()
{
  Serial.begin(230400);

  //Set B pins to input
  DDRB = B00000000;
  delay(1000);
}

void loop()
{
  takeSample();
}

void takeSample()
{
  byte pinInput1 = 0;
  byte outByte = 0;
  byte currentByte = 0;
  byte mixedByte1 = 0;
  byte mixedByte2 = 0;
  byte mixedByte3 = 0;
  byte mixedByte4 = 0;

  //Loop for building a byte from bits 
  for(int j=0; j < 8; j++){ 
    //Pins are read using registers because its much faster than digitalRead()
    pinInput1 = (PINB >> 3) & B00000001;
    //Add the bit to currentByte
    currentByte = (pinInput1 << j) | currentByte;
  }  

  //Mix the current byte with the previous one
  mixedByte1 = currentByte ^ previousByte;
  mixedByte2 = mixedByte1 ^ previousMixedByte1;
  mixedByte3 = mixedByte2 ^ previousMixedByte2;
  mixedByte4 = mixedByte3 ^ previousMixedByte3;
  outByte = mixedByte4 ^ previousMixedByte4;
  //Send it to your computer
  Serial.write(outByte);  
  
  previousByte = currentByte;
  previousMixedByte1 = mixedByte1;
  previousMixedByte2 = mixedByte2; 
  previousMixedByte3 = mixedByte3;
  previousMixedByte4 = mixedByte4;
}  




