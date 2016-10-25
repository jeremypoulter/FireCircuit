#include <Adafruit_CircuitPlayground.h>
#include <Adafruit_SleepyDog.h>

void setup() {
  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(255); // LEDs full blast!
}

#define CAP_THRESHOLD    300  // Threshold for a capacitive touch (higher = less sensitive).
#define CAP_SAMPLES      20   // Number of samples to take for a capacitive touch read.

uint8_t prev[] = {128, 128};              // Start brightness in middle

void loop() 
{
  if(CircuitPlayground.slideSwitch())
  {
    if (CircuitPlayground.readCap(0, CAP_SAMPLES) >= CAP_THRESHOLD) 
    {
      uint32_t offset = millis() / 5;
      // Loop through each pixel and set it to an incremental color wheel value.
      for(int i=0; i<10; ++i) {
        CircuitPlayground.strip.setPixelColor(i, CircuitPlayground.colorWheel(((i * 256 / 10) + offset) & 255));
      }
      // Show all the pixels.
      CircuitPlayground.strip.show();
    }
    else
    {
      for (int i = 0; i < 2; i++)
      {
        uint8_t lvl = random(64, 192);            // End brightness at 128±64
        split(prev[i], lvl, 32, i*5, 5 + (i*5));  // Start subdividing, ±32 at midpoint
        prev[i] = lvl;                            // Assign end brightness to next start
      }
    }
    delay(4);
  }
  else
  {
    for(uint8_t i=0; i<10; i++) CircuitPlayground.strip.setPixelColor(i, 0);
    CircuitPlayground.strip.show();
    Watchdog.sleep(1000);
  }
}

void split(uint8_t y1, uint8_t y2, uint8_t offset, int start, int end) {
  if(offset) { // Split further into sub-segments w/midpoint at ±offset
    uint8_t mid = (y1 + y2 + 1) / 2 + random(-offset, offset);
    split(y1 , mid, offset / 2, start, end); // First segment (offset is halved)
    split(mid, y2 , offset / 2, start, end); // Second segment (ditto)
  } else { // No further subdivision - y1 determines LED brightness
    uint32_t c = (((int)(pow((float)y1 / 255.0, 2.7) * 255.0 + 0.5) // Gamma
                 * 0x1004004) >> 8) & 0xFF3F03; // Expand to 32-bit RGB color
    for(uint8_t i=start; i<end; i++) CircuitPlayground.strip.setPixelColor(i, c);
    CircuitPlayground.strip.show();
  }
}