#ifndef LEDARDUINO_H
#define LEDARDUINO_H

#include <Adafruit_DotStar.h>
#include <SPI.h>

#define NUM_PIXELS 256
#define DATA_PIN 11
#define CLOCK_PIN 13
#define INTERRUPT_PIN 2


class LEDArduino
{
    private:
        Adafruit_DotStar strip;

        // static variables

        // stores LEDs for various patterns
        static byte leftLEDs[117], rightLEDs[117], topLEDs[123], bottomLEDs[116], ringLEDs[48], centralLEDs[39];
        static byte* ledPatterns[6];
        static byte patternLength[6];

        // stores total number of LEDs 
        byte totalNumberOfLEDs;

        // stores total number of patterns
        byte totalNumberOfPatterns;

        // stores color of the LEDs
        uint32_t color;

        // stores the LED indices
        byte* leds;

        // stores pattern indices
        byte* patterns;

        // temporary variables to count number of LEDS and patters loaded from the serial port
        byte countLEDs, countPatterns;

        // flag to enable/disable increment of the counter
        byte enableCounterIncrement;

        // counter
        byte counter;

        // -------------private functions----------------------

       
        // turns all LEDs in the arr array
        void turnArray(byte* arr, int len);

    public:
        // constructor
        LEDArduino();

        // destructor
        ~LEDArduino();

        // interrupt service for advancing the hardware counter
        void isrAdvance();

        // resets the LED display and clears memory (if any)
        void reset();

        //sets the power level of the LEDs
        void setPower(byte power);
        
        // advances LED sequence
        void advance();

        // reads command from serial port and executes
        void executeSerialCommand();

        // Prints information about the LEDArduino 
        void getInfo();

        // returns power
        byte getPower();

};

#endif
