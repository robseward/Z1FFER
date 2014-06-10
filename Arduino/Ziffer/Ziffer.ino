
#define READ_PIN 7

void setup()
{
  Serial.begin(230400);

  DDRD = B00000000;
  DDRB = B00000000;
  delay(1000);
}


byte previousByte = 0;
  
void takeSample()
{
  byte pinInput1 = 0;
  byte pinInput2 = 0;
  byte combinedInput = 0;
  byte outbyte = 0;
  byte b = 0;
 
  for(int j=0; j < 8; j++){ 
    //Pins are read using registers because its much faster
    pinInput1 = (PINB >> 3) & B00000001;
    pinInput2 = (PINB >> 4) & B00000001;

    combinedInput = pinInput1 ^ pinInput2;
    
    b = (combinedInput << j) | b;
  }  

  outbyte = b ^ previousByte;
  Serial.write(outbyte);  
  
  previousByte = b;
  //For eyeball debugging
  //delay(100);
  //Serial.println(b, BIN);
}  

void loop()
{
  takeSample();
}


