
byte previousByte = 0;

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
  byte pinInput2 = 0;
  byte combinedInput = 0;
  byte outbyte = 0;
  byte currentByte = 0;
 
  //For loop for building a byte from bits 
  for(int j=0; j < 8; j++){ 
    //Pins are read using registers because its much faster than digitalRead()
    pinInput1 = (PINB >> 3) & B00000001;
    pinInput2 = (PINB >> 4) & B00000001;

    //Mix the two Z1FFER bits
    combinedInput = pinInput1 ^ pinInput2;
    
    //Add the bit in combinedInput to currentByte
    currentByte = (combinedInput << j) | currentByte;
  }  

  //Mix the current byte with the previous one
  outbyte = currentByte ^ previousByte;
  Serial.write(outbyte);  
  
  previousByte = currentByte;
}  




