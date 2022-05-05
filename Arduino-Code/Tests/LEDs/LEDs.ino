// ! <-- Include required libraries --> !
#include <Adafruit_NeoPixel.h>

// --> Include required libraries <--

// ! <-- Communication-needed devices settings --> !

int
//LED 1 (H1)
H1_pin = 0,
H1_numOfLeds = 51,

//LED 2 (H2)
H2_pin = 1, 
H2_numOfLeds = 6,

//LED 3 (H3)
H3_pin = 2, 
H3_numOfLeds = 3;

// Link LEDs
Adafruit_NeoPixel H1(H1_numOfLeds, H1_pin, NEO_GRB + NEO_KHZ800); // Link H1 to NeoPixel lib
Adafruit_NeoPixel H2(H2_numOfLeds, H2_pin, NEO_GRB + NEO_KHZ800); // Link H2 to NeoPixel lib
Adafruit_NeoPixel H3(H3_numOfLeds, H3_pin, NEO_GRB + NEO_KHZ800); // Link H3 to NeoPixel lib
// --> Communication-needed devices settings <--

void setup() {
  // ! <-- Devices and pins init --> !

  
  H1.begin(); H2.begin(); H3.begin(); 
  H1.clear(); H2.clear(); H3.clear();
  H1.setBrightness(200); H2.setBrightness(200); H3.setBrightness(200);
  // --> Devices and pins init <--
}

void loop() {
  rainbow(10, H1);
  rainbow(10, H2);
  rainbow(10, H3);
}

void rainbow(int wait, Adafruit_NeoPixel H) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    H.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    H.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}
