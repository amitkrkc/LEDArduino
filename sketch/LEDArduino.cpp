#include "LEDArduino.h"
#include "Adafruit_DotStar.h"

void LEDArduino:: deleteLEDs()
{
  for(byte i=0;i<numRows;++i)
    delete[] leds[i];
  delete[] leds;
}

// interrupt service for advancing the hardware counter
void LEDArduino::isrAdvance()
{
    if(enableCounterIncrement)
        advance();
}

// turns all LEDs in the arr array
void LEDArduino::turnArray(byte* arr, int len)
{
    strip.clear();
    for(int i=0;i<len;++i)
    {
        if(arr[i]==254)// ignore the 254-the LED
          continue;
        strip.setPixelColor(arr[i],color);
    }
    strip.show();
}

// Prints information about the LEDArduino
void LEDArduino::getInfo()
{
    Serial.write(counter);
    Serial.write(enableCounterIncrement);
    Serial.write(color & 255);
    Serial.write(numRows);
    Serial.write(numCols);
    for(byte i=0;i<numRows;++i)
      Serial.write(leds[i],numCols);
    Serial.write(totalNumberOfPatterns);
    Serial.write(patterns, totalNumberOfPatterns);

}

byte LEDArduino::getPower()
{
    return byte(color & 255);
}

// constructor
LEDArduino::LEDArduino():strip(NUM_PIXELS, DATA_PIN, CLOCK_PIN, DOTSTAR_BRG)
{
    strip.begin();
    strip.show();

    enableCounterIncrement=0; // initially disabled
    counter=0; // initially set to zero
    totalNumberOfPatterns=0;
    numRows=0;
    numCols=0;

}

// copy constructor
LEDArduino::LEDArduino(const LEDArduino& obj):strip(NUM_PIXELS, DATA_PIN, CLOCK_PIN, DOTSTAR_BRG)
{
  strip.begin();
  strip.show();
  enableCounterIncrement=obj.enableCounterIncrement;
  counter=obj.counter;
  totalNumberOfPatterns=obj.totalNumberOfPatterns;
  numRows=obj.numRows;
  numCols=obj.numCols;
}

// destructor
LEDArduino::~LEDArduino()
{
    reset();
}

// resets the LED display and clears memory (if any)
void LEDArduino::reset()
{
    strip.clear();
    strip.show();
    enableCounterIncrement=0;
    counter=0;
    totalNumberOfPatterns=0;
    numRows=0;
    numCols=0;

    if(leds)
        deleteLEDs();
    if(patterns)
        delete[] patterns;
}

//sets the power level of the LEDs
void LEDArduino::setPower(byte power)
{
    color=((uint32_t)power << 16) | ((uint32_t)power << 8) | (uint32_t)power;
}

// advance the counter
void LEDArduino::advance()
{
    if(counter<numRows)
    {
        turnArray(leds[counter],numCols);
    }
    else
    {
        turnArray(ledPatterns[counter-numRows],patternLength[counter-numRows]);
    }
    counter=(counter+1)%(numRows+totalNumberOfPatterns);
}

void LEDArduino::executeSerialCommand()
{

    if(Serial.available()>0)
    {
        //peek the command character
        char cmd=Serial.read();
        switch(cmd)
        {
            //blink
            case 'b':
              byte num_delay, delay_val;
              Serial.readBytes(&num_delay,1);
              Serial.readBytes(&delay_val,1);
              for(byte i=0;i<num_delay;++i)
              {
                digitalWrite(LED_BUILTIN, HIGH);
                delay(delay_val);
                digitalWrite(LED_BUILTIN, LOW);
                delay(delay_val);
              }
              break;

            // advance
            case 'a':
                advance();
                break;

            // reset
            case 'r':
                reset();
                break;

            // zero out the LEDs
            case 'z':
                strip.clear();
                strip.show();
                break;

            // individual LED
            case 'i':
                byte ind;
                Serial.readBytes(&ind,1);   // read the LED
                if(ind>=0 && ind<=NUM_PIXELS)
                {
                    strip.clear();
                    strip.setPixelColor(ind,color);
                    strip.show();
                }
                break;

            // LED array
            case 'l':
                byte num_leds;
                Serial.readBytes(&num_leds,1);
                byte* arr;
                arr=new byte[num_leds];
                Serial.readBytes(arr,num_leds);
                turnArray(arr, int(num_leds));
                delete[] arr;
                break;

            // turn leds from start to end
            case 'c':
                byte start_led,end_led;
                Serial.readBytes(&start_led,1);
                Serial.readBytes(&end_led,1);
                strip.clear();
                for(byte i=start_led;i<=end_led;++i)
                {
                    strip.setPixelColor(i,color);
                }
                strip.show();
                break;

            // get current counter
            case 'g':
                Serial.write(counter);
                break;

            // get current power
            case 'G':
                Serial.write(getPower());
                break;

            // set counter
            case 's':
                Serial.readBytes(&counter,1);
                break;

            // enable or disable hardware counter
            case 'e':
                Serial.readBytes(&enableCounterIncrement,1);
                break;

            //set power
            case 'w':
                byte pwr;
                Serial.readBytes(&pwr,1);
                setPower(pwr);
                break;

            //get info
            case 'v':
                getInfo();
                break;

            // turn pattern
            case 'p':
                byte pattern;
                Serial.readBytes(&pattern,1);
                if(pattern>=0 && pattern<=5)
                    turnArray(ledPatterns[pattern], patternLength[pattern]);
                break;

            // load number of LEDs and patterns
            case 'n':
                // read code for LED or pattern
                byte isLED;
                Serial.readBytes(&isLED,1);
                if(isLED==1)
                {
                    Serial.readBytes(&numRows,1);
                    Serial.readBytes(&numCols,1);
                    if(numRows>0 && numCols>0)
                    {
                      if(leds)
                        deleteLEDs();

                      leds=new byte*[numRows];
                      for(byte i=0;i<numRows;++i)
                        leds[i]=new byte[numCols];
                    }
                    countLEDs=0;
                }
                else if(isLED==2) //patterns
                {
                    Serial.readBytes(&totalNumberOfPatterns,1);
                    if(totalNumberOfPatterns>0)
                    {
                      if(patterns)
                        delete[] patterns;
                      patterns=new byte[totalNumberOfPatterns];
                    }
                    countPatterns=0;
                }
                break;

            case 'L':
                if(numRows>0 && numCols>0)
                {
                    if(countLEDs<numRows)
                    {
                        Serial.readBytes(leds[countLEDs++],numCols);
                    }
                }
                counter=0;
                break;

            case 'P':
                if(totalNumberOfPatterns>0)
                {
                    Serial.readBytes(patterns,totalNumberOfPatterns);
                }
                counter=0;
                break;
        }//end switch
    }//end if Serial.available()>0
}//end of executeSerialCommand()



// initialize static variables
byte LEDArduino::leftLEDs[117]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,49,50,51,52,53,54,55,
56,57,58,59,60,61,62,63,64,65,66,67,68,69,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,
110,111,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,165,166,167,168,169,170,
171,172,173,174,175,176,192,193,194,195,196,197,198,199,200,201,202,216,217,218,219,220,
221,222,223,224,236,237,238,239,240,248,249};

byte LEDArduino::rightLEDs[117] = {25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,71,72,73,74,75,76,77,
78,79,80,81,82,83,84,85,86,87,88,89,90,91,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,
130,131,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,179,180,181,182,183,184,185,186,187,188,189,
190,204,205,206,207,208,209,210,211,212,213,214,226,227,228,229,230,231,232,233,234,242,243,244,245,246,251,252};

byte LEDArduino::topLEDs[123] ={0,1,2,3,4,5,6,7,8,9,10,11,12,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,
51,52,53,54,55,56,57,58,59,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,
122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,156,157,158,159,
160,161,162,163,164,165,166,167,168,169,170,171,184,185,186,187,188,189,190,191,192,193,194,195,
196,197,230,231,232,233,234,235,236,237,238,244,245,246,247,248,252};

byte LEDArduino::bottomLEDs[115] = {13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,60,61,62,63,64,65,66,67,68,69,70,
71,72,73,74,75,76,77,78,79,80,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,141,142,143,
144,145,146,147,148,149,150,151,152,153,154,155,172,173,174,175,176,177,178,179,180,181,182,183,198,199,200,201,202,203,204,
205,206,207,208,221,222,223,224,225,226,227,228,229,239,240,241,242,243};

byte LEDArduino::centralLEDs[39]= {215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253};
byte LEDArduino::ringLEDs[48] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47};


byte LEDArduino::patternLength[6]={117,117,123,115,39,48};
byte* LEDArduino::ledPatterns[6]={leftLEDs,rightLEDs,topLEDs,bottomLEDs,centralLEDs,ringLEDs};


