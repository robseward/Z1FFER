/*****************************
 * Z1FFER
 * OpenRandom.org
 *
 * Created by Rob Seward 2013-2014
 * 2013-2014 VHS Design LLC
/******************************/
 
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
  byte outbyte = 0;
  byte currentByte = 0;
 
  //Loop for building a byte from bits 
  for(int j=0; j < 8; j++){ 
    //Pins are read using registers because its much faster than digitalRead()
    pinInput1 = (PINB >> 3) & B00000001;
    
    //Add the bit to currentByte
    currentByte = (pinInput1 << j) | currentByte;
  }  

  //Mix the current byte with the previous one
  outbyte = currentByte ^ previousByte;
  
  //Send it to your computer
  Serial.write(outbyte);  
  
  previousByte = currentByte;
}  




