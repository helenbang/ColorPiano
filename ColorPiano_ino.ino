// ColorPiano
// adapted from the Adafruit's color sensor tutorial and the Flora Pianoglove tutorial. 

=========================
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "Flora_Pianoglove.h"
#include <SoftwareSerial.h>

// we only play a note when the clear response is higher than a certain number 
#define CLEARTHRESHHOLD 2000
#define LOWTONE 1000
#define HIGHTONE 2000
#define LOWKEY 64   // high C
#define HIGHKEY 76  // double high C

// our RGB -> eye-recognized gamma color
byte gammatable[256];

int prevNote = -1;

// color sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(9600);
  
  //Check for color sensor
  if (tcs.begin()) {
    // Serial.println("Found sensor");
  } 
  else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    gammatable[i] = x;      
  }
}

void loop() {
  uint16_t clear, red, green, blue;

  tcs.setInterrupt(false);      // turn on LED

  delay(60);  // takes 50ms to read 

  tcs.getRawData(&red, &green, &blue, &clear);

  tcs.setInterrupt(true);  // turn off LED

  //  Serial.print("C:\t"); Serial.print(clear);
  //  Serial.print("\tR:\t"); Serial.print(red);
  //  Serial.print("\tG:\t"); Serial.print(green);
  //  Serial.print("\tB:\t"); Serial.print(blue);

  // Figure out some basic hex code for visualization
  uint32_t sum = red;
  sum += green;
  sum += blue;
  sum = clear;
  float r, g, b;
  r = red; 
  r /= sum;
  g = green; 
  g /= sum;
  b = blue; 
  b /= sum;
  r *= 256; 
  g *= 256; 
  b *= 256;
  if (r > 255) r = 255;
  if (g > 255) g = 255;
  if (b > 255) b = 255;

  //  Serial.print("\t");  // what is read on processing!!!
  //  Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b,    
      HEX); 
  //  Serial.println();


  // OK we have to find the two primary colors
  // check if blue is smallest. MEME: fix for 'white'
  float remove, normalize;
  if ((b < g) && (b < r)) {
    remove = b;
    normalize = max(r-b, g-b);
  } 
  else if ((g < b) && (g < r)) {
    remove = g;
    normalize = max(r-g, b-g);
  } 
  else {
    remove = r;
    normalize = max(b-r, g-r);
  }
  // get rid of minority report
  float rednorm = r - remove;
  float greennorm = g - remove;
  float bluenorm = b - remove;
  // now normalize for the highest number
  rednorm /= normalize;
  greennorm /= normalize;
  bluenorm /= normalize;

  //  Serial.println();
  //  strip.setPixelColor(0, strip.Color(gammatable[(int)r], gammatable[(int)g], 
      gammatable[(int)b]));
  //  strip.show();

  //  Serial.print(rednorm); Serial.print(", "); 
  //  Serial.print(greennorm); Serial.print(", "); 
  //  Serial.print(bluenorm); Serial.print(" "); 
  //  Serial.println();

  float rainbowtone = 0;

  if (bluenorm <= 0.1) {
    // between red and green
    if (rednorm >= 0.99) {
      // between red and yellow
      rainbowtone = 0 + 0.2 * greennorm;
    } 
    else {
      // between yellow and green
      rainbowtone = 0.2 + 0.2 * (1.0 - rednorm);
    }
  } 
  else if (rednorm <= 0.1) {
    // between green and blue
    if (greennorm >= 0.99) {
      // between green and teal
      rainbowtone = 0.4 + 0.2 * bluenorm;
    } 
    else {
      // between teal and blue
      rainbowtone = 0.6 + 0.2 * (1.0 - greennorm);
    }
  } 
  else {
    // between blue and violet
    if (bluenorm >= 0.99) {
      // between blue and violet
      rainbowtone = 0.8 + 0.2 * rednorm;
    } 
    else {
      // between teal and blue
      rainbowtone = 0; 
    }
  }

  //  Serial.print("Scalar "); Serial.println(rainbowtone);
  float keynum = LOWKEY + (HIGHKEY - LOWKEY) * rainbowtone;
  Serial.print(keynum); Serial.print(","); Serial.println();
  //  float freq = pow(2, (keynum - 49) / 12.0) * 440;
  //  Serial.print("Freq = "); Serial.println(freq);  
  //  Serial.print((int)r ); Serial.print(" "); Serial.print((int)g);Serial.print(" 
      ");  Serial.println((int)b );
  //  playNote(keynum);
}
