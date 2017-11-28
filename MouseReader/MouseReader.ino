

#define SCLK 5
#define SDIO 6
#define COUNT 4

#define FRAMELENGTH 324
byte frame[FRAMELENGTH];

const byte regConfig    = 0x00;
const byte regStatus    = 0x01;
const byte regXmov      = 0x02;
const byte regYmov      = 0x03;
const byte regPixelData = 0x08;
const byte maskNoSleep  = 0x01;
const byte maskPID      = 0xE0;

byte flop;

void setup()
{
  pinMode(SCLK, OUTPUT);
  for( int i = 0; i < COUNT; i++) 
    pinMode(SDIO + i, OUTPUT); 

  flop = false;

  Serial.begin(115200);
  //Serial.println("Serial established.");
  //Serial.flush();

  mouseInit();
  //dumpDiag();
}

void loop()
{
  unsigned int s;
  int input;
  
  byte buffX[COUNT];
  byte buffY[COUNT];

  //readFrame(frame, buff);

  if( Serial.available() )
  {
    /*
    input = Serial.read();
    switch( 'd' )
    {
    case 'f':
      Serial.println("Frame capture.");
      readFrame(frame);
      Serial.println("Done.");
      break;
    case 'd':
      for( input = 0; input < FRAMELENGTH; input++ )  //Reusing 'input' here
        Serial.print( (byte) frame[input] );
      Serial.print( (byte)127 );
Serial.print("\n");
      break;
      
case 'x':  // read X movement register
buff = readRegister(regXmov);
Serial.print((byte) buff);
Serial.print("\n");
break;
case 'y':  // read Y movement register
buff = readRegister(regYmov);
Serial.print((byte) buff);
Serial.print("\n");
break;

case 's':
buff = readRegister(regStatus);
Serial.print((byte) buff);
Serial.print("\n");
break;
    }
    */

    readRegister(regXmov, buffX);
    readRegister(regYmov, buffY);
    for( int i = 0; i < COUNT; i++ )
    {
      Serial.print(i);
      Serial.print(":");
      Serial.print((byte) buffX[i]);
      Serial.print(",");
      Serial.print((byte) buffY[i]);
      Serial.print(";");
    }
    Serial.print("\n");
  }
}


/*  
 Serial driver for ADNS2010, by Conor Peterson (robotrobot@gmail.com)
 Serial I/O routines adapted from Martjin The and Beno?t Rosseau's work.
 Delay timings verified against ADNS2061 datasheet.
 
 The serial I/O routines are apparently the same across several Avago chips.
 It would be a good idea to reimplement this code in C++. The primary difference
 between, say, the ADNS2610 and the ADNS2051 are the schemes they use to dump the data
 (the ADNS2610 has an 18x18 framebuffer which can't be directly addressed).
 
 This code assumes SCLK is defined elsewhere to point to the ADNS's serial clock,
 with SDIO pointing to the data pin.
*/


void mouseInit(void)
{
  digitalWrite(SCLK, HIGH);
  delayMicroseconds(5);
  digitalWrite(SCLK, LOW);
  delayMicroseconds(1);
  digitalWrite(SCLK, HIGH);
  delay(1025);
  writeRegister(regConfig, maskNoSleep); //Force the mouse to be always on.
}

void dumpDiag(void)
{
  unsigned int val;
  byte buf[COUNT];
  readRegister(regStatus, buf);

  for( int i = 0; i < COUNT; i++ )
  {
    Serial.print("Product ID: ");
    Serial.println( (unsigned int)((val & maskPID) >> 5));
  }
  
  Serial.println("Ready.");
  Serial.flush();
}

void writeRegister(byte addr, byte data)
{
  byte i;

  addr |= 0x80; //Setting MSB high indicates a write operation.

  //Write the address
  for( int i = 0; i < COUNT; i++) 
    pinMode(SDIO + i, OUTPUT);
    
  for (i = 8; i != 0; i--)
  {
    digitalWrite (SCLK, LOW);
    for( int i = 0; i < COUNT; i++) 
      digitalWrite(SDIO + i, addr & (1 << (i-1) ));
    digitalWrite (SCLK, HIGH);
  }

  //Write the data    
  for (i = 8; i != 0; i--)
  {
    digitalWrite (SCLK, LOW);
    for( int i = 0; i < COUNT; i++) 
      digitalWrite(SDIO + i, data & (1 << (i-1) ));
    digitalWrite (SCLK, HIGH);
  }
}

void readRegister(byte addr, byte* r)
{
  byte j;
  for( int i = 0; i < COUNT; i++) 
    r[i] = 0;
  

  //Write the address
  for( int i = 0; i < COUNT; i++) 
    pinMode(SDIO + i, OUTPUT); 
  for (j = 8; j != 0; j--)
  {
    digitalWrite (SCLK, LOW);
    for( int i = 0; i < COUNT; i++) 
      digitalWrite(SDIO + i, addr & (1 << (j-1) ));
    digitalWrite (SCLK, HIGH);
  }

  for( int i = 0; i < COUNT; i++) 
    pinMode(SDIO + i, INPUT); 
  delayMicroseconds(110);  //Wait (per the datasheet, the chip needs a minimum of 100 µsec to prepare the data)

  //Clock the data back in
  for (j = 8; j != 0; j--)
  {                             
    digitalWrite (SCLK, LOW);
    digitalWrite (SCLK, HIGH);
    for( int i = 0; i < COUNT; i++) 
      r[i] |= (digitalRead (SDIO + i) << (j-1) );
  }

  delayMicroseconds(110);  //Tailing delay guarantees >100 µsec before next transaction
}


//ADNS2610 dumps a 324-byte array, so this function assumes arr points to a buffer of at least 324 bytes.
/*void readFrame(byte *arr)
{
  byte *pos;
  byte *uBound;
  unsigned long timeout;
  byte val;

  //Ask for a frame dump
  writeRegister(regPixelData, 0x2A);

  val = 0;
  pos = arr;
  uBound = arr + 325;

  timeout = millis() + 1000;

  //There are three terminating conditions from the following loop:
  //1. Receive the start-of-field indicator after reading in some data (Success!)
  //2. Pos overflows the upper bound of the array (Bad! Might happen if we miss the start-of-field marker for some reason.)
  //3. The loop runs for more than one second (Really bad! We're not talking to the chip properly.)
  while( millis() < timeout && pos < uBound)
  {
    val = readRegister(regPixelData);

    //Only bother with the next bit if the pixel data is valid.
    if( !(val & 64) )
      continue;

    //If we encounter a start-of-field indicator, and the cursor isn't at the first pixel,
    //then stop. ('Cause the last pixel was the end of the frame.)
    if( ( val & 128 ) 
      &&  ( pos != arr) )
      break;

    *pos = val & 63;
    pos++;
  }

}*/

