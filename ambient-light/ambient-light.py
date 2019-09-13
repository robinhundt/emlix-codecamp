#!/usr/bin/env python3

# NeoPixel library strandtest example
# Author: Tony DiCola (tony@tonydicola.com)
#
# Direct port of the Arduino NeoPixel library strandtest example.  Showcases
# various animations on a strip of NeoPixels.

import time
from rpi_ws281x import PixelStrip, Color
import argparse
from os import environ

SYSFS_TEMPERATURE_FILE = environ.get("SYSFS_TEMPERATURE_FILE", "/sys/devices/platform/soc/20804000.i2c/i2c-1/1-0076/temperature")
MIN_TEMP = 0
MAX_TEMP = 30 * 100
# LED strip configuration:
LED_COUNT = 12        # Number of LED pixels.
LED_PIN = 18          # GPIO pin connected to the pixels (18 uses PWM!).
# LED_PIN = 10        # GPIO pin connected to the pixels (10 uses SPI /dev/spidev0.0).
LED_FREQ_HZ = 800000  # LED signal frequency in hertz (usually 800khz)
LED_DMA = 10          # DMA channel to use for generating signal (try 10)
LED_BRIGHTNESS = 55  # Set to 0 for darkest and 255 for brightest
LED_INVERT = False    # True to invert the signal (when using NPN transistor level shift)
LED_CHANNEL = 0       # set to '1' for GPIOs 13, 19, 41, 45 or 53


# Define functions which animate LEDs in various ways.
def colorWipe(strip, color, wait_ms=50):
    """Wipe color across display a pixel at a time."""
    for i in range(strip.numPixels()):
        strip.setPixelColor(i, color)
        strip.show()
        time.sleep(wait_ms / 1000.0)

# Main program logic follows:
if __name__ == '__main__':
    # Process arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--clear', action='store_true', help='clear the display on exit')
    args = parser.parse_args()

    # Create NeoPixel object with appropriate configuration.
    strip = PixelStrip(LED_COUNT, LED_PIN, LED_FREQ_HZ, LED_DMA, LED_INVERT, LED_BRIGHTNESS, LED_CHANNEL)
    # Intialize the library (must be called once before other functions).
    strip.begin()

    print('Press Ctrl-C to quit.')
    if not args.clear:
        print('Use "-c" argument to clear LEDs on exit')

    try:

        while True:
            #print('Color wipe animations.')
            #colorWipe(strip, Color(255, 0, 0))  # Red wipe
            #colorWipe(strip, Color(0, 255, 0))  # Blue wipe
            #print('Theater chase animations.')
            #theaterChase(strip, Color(127, 127, 127))  # White theater chase
            #theaterChase(strip, Color(127, 0, 0))  # Red theater chase
            #theaterChase(strip, Color(0, 0, 127))  # Blue theater chase
            #print('Rainbow animations.')
            #rainbow(strip)
            #rainbowCycle(strip)
            #theaterChaseRainbow(strip)

            temp = int(open(SYSFS_TEMPERATURE_FILE).read())
            temp = max(MIN_TEMP, min(MAX_TEMP, temp))

            red = [55, 0, 0]
            blue = [0, 0, 55]

            nsteps = 55

            color = [None, None, None]
            for i in [0, 1, 2]:
                color[i] = int(blue[i] + ((temp - MIN_TEMP) / (MAX_TEMP - MIN_TEMP)) * (red[i] - blue[i]))

            colorWipe(strip, Color(*color), wait_ms=0)            
            time.sleep(0.2)

    except KeyboardInterrupt:
        if args.clear:
            colorWipe(strip, Color(0, 0, 0), 10)
