#ifndef LEDARDUINO_H
#define LEDARDUINO_H

#include "Adafruit_DotStar.h"
#include <SPI.h>

#define NUM_PIXELS 256
#define DATA_PIN 11
#define CLOCK_PIN 13
#define INTERRUPT_PIN 2

/*!
LEDArduino class to interface Adafruit dotstar LED disk
*/
class LEDArduino
{
    private:
        /// LED disk
        Adafruit_DotStar strip;


        // static variables for storing LEDs for various patterns
        static byte leftLEDs[117], rightLEDs[117], topLEDs[123], bottomLEDs[115], ringLEDs[48], centralLEDs[39];
        static byte* ledPatterns[6];
        static byte patternLength[6];
        

        /// stores total number of LEDs
        byte numRows, numCols;

        /// stores total number of patterns
        byte totalNumberOfPatterns;

        /// stores color of the LEDs
        uint32_t color;

        /// stores the LED indices
        byte** leds;

        /// stores pattern indices
        byte* patterns;

        // temporary variables to count number of LEDS and patters loaded from the serial port
        byte countLEDs, countPatterns;

        /// flag to enable/disable increment of the counter
        byte enableCounterIncrement;

        /// counter
        byte counter;

        // -------------private functions----------------------


        /// turns all LEDs in the arr array
        void turnArray(byte* arr, int len);

        ///helper function
        void deleteLEDs();

    public:
        /// default constructor
        LEDArduino();

        /*!
        Copy constructor
        @param obj : a reference to LEDArduino object (const)
        */
        LEDArduino(const LEDArduino& obj);

        /// destructor
        ~LEDArduino();

        /// interrupt service for advancing the hardware counter
        void isrAdvance();

        /// resets the LED display and clears memory (if any)
        void reset();

        /*!
        @brief sets the power level of the LEDs
        @param power : a byte variable to specify the brightness of the LEDs
        */
        void setPower(byte power);

        /// advances LED sequence
        void advance();

        /// reads commands from serial port and executes them
        void executeSerialCommand();

        /// Prints information about the LEDArduino
        void getInfo();

        /*!
        @brief returns brightness value or power
        @retval power : a byte variable
        */
        byte getPower();

};

#endif
