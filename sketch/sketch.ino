#include "LEDArduino.h"

LEDArduino obj=LEDArduino();

void isrAdvance()
{
  obj.isrAdvance();
}
void setup()
{
  // set-up 
  Serial.begin(115200);

    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
        clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
    #endif
    
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN),isrAdvance,RISING);
  
}

void loop()
{
  obj.executeSerialCommand();
}

