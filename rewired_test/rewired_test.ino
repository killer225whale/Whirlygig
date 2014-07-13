#include <Servo.h>

Servo ESC;

int dataPin = A0;        //Define which pins will be used for the Shift Register control
int latchPin = A1;
int clockPin = A2;

int PIN_DigitalMap[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5};

void CshiftOut(uint16_t);

int bitcount = 0;
int prev, i;

uint16_t seq1[30] = {
  1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,16384,8192,4096,2048,1024,512,256,128,64,32,16,8,4,2};
uint16_t seq2[14] = {
32769,16386,8196,4104,2064,1056,576,384,576,1056,2064,4104,8196,16386};
uint8_t seq3[14] = {
  128,64,32,16,8,4,2,1,2,4,8,16,32,64};

void setup()
{
  pinMode(dataPin, OUTPUT);       //Configure each IO Pin
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  //Serial.begin(9600);
  
  ESC.attach(11);
  ESC.write(10);
  delay(5000);
  ESC.write(100);
}

void loop()
{
  /*
  prev = millis();
   for (int i = 0; i < 100; i++)
   {
   */
  for (int x = 0; x < 14; x++)         //Array Index
  {
    PORTC &= ~(1 << 1);
    for(i = 0; i < 30; i++)
      CshiftOut(seq2[x]);
      delayMicroseconds(10);
    PORTC |= (1 << 1);
    delay(40);
  }
  /*
  }
   Serial.println(millis()-prev);
   */
}

void CshiftOut(uint16_t val)
{
  for(bitcount = 0; bitcount < 16; bitcount++)
  {
    if(bitRead(val, bitcount))
      PORTC |= (1 << 0);
    else
      PORTC &= ~(1 << 0);
    PORTC |= (1 << 2);
    PORTC &= ~(1 << 2);
  }
}



















