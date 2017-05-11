# LEDArduino
C++ and MATLAB interface for Adafruit LED disk. This is currently in 'super' development mode.

Currently it supports turning individual LED or a range of LEDs. The sequence of patterns can be iterated either by issuing an 'advance' command from MATLAB or from the interrupt on pin 2. The interrupt option has been used to automatically turn next LED by monitoring the TTL (fire) signal from Zyla camera. 
