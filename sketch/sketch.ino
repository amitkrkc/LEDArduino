#include "LEDArduino.h"

LEDArduino obj;

void isrAdvance()
{
  obj.isrAdvance();
}
void setup()
{
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN),isrAdvance,RISING);
  
}

void loop()
{
  obj.executeSerialCommand();
}

