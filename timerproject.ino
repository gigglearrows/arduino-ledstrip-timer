#include <AceButton.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

using namespace ace_button;

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// Input pin for button
#define BUTTONPIN      4

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      92

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Global vars
int MINUTES = 1;
uint32_t COLORARRAY[NUMPIXELS]; // Used for the ledblink function at the end to remember prev. pattern
uint32_t GREEN = pixels.Color(0, 150, 0);
uint32_t YELLOW = pixels.Color(150, 150, 0);
uint32_t RED = pixels.Color(150, 0, 0);

bool should_run;

// Functions
void run_timer();
void ledblink();
void blackout();
void cleanup();
void set_colors(uint32_t color, int num_leds_on);
void handleEvent(AceButton*, uint8_t, uint8_t);

// Classes
AceButton powerbutton(BUTTONPIN);


void setup() {
  should_run = false; // Start stopped
  
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(32);
  pixels.show();

  pinMode(BUTTONPIN, INPUT_PULLUP);
  powerbutton.setEventHandler(handleEvent);

  // Uncomment for debugging
  Serial.begin(9600);
}

// Main loop function
void loop() {
  //delay(5000);
  powerbutton.check();

  if (should_run) { // If button has just been pressed
    //delay(10); // Crude form of button debouncing
    run_timer();
  }
}


void run_timer() {
  uint32_t color = GREEN;
  unsigned long starttime = millis();
  unsigned long endtime = starttime + (60 * 100L * MINUTES); //convert minutes to milliseconds
  unsigned long totaltime = endtime - starttime;
  double percentdelta = 0;
  unsigned long curtime = starttime;
  unsigned long timedelta;
  int num_leds_on;
  double percent;

  while (curtime < endtime) {
    // cleanup and break if button is pressed.
    if (!should_run) {
      cleanup();
      break;
    }
    
    curtime = millis();
    timedelta = curtime - starttime;
    percent = (double)timedelta / totaltime;
    num_leds_on = (int)((percent - percentdelta) * 3 * NUMPIXELS);
    //set_colors(color, num_leds_on);
    //Serial.println(num_leds_on % 23);
    if (num_leds_on % 4 == 0) {
      set_colors(color, num_leds_on);
    }

    if (percent == (double)1.0/3.0 || percent == (double)2.0/3.0) {
      num_leds_on = 0;
    }
    if (percent >= (double)1.0) {
      ledblink();
    } else if (percent > (double)2.0/3.0) {
      color = RED;
      percentdelta = (double)2.0/3.0;
    } else if (percent > (double)1.0/3.0) {
      color = YELLOW;
      percentdelta = (double)1.0/3.0;
    }

    // Uncomment for debugging
    //Serial.print(timedelta/1000);
    //Serial.print("s of ");
    //Serial.print(totaltime/1000);
    //Serial.print("s elapsed (");
    //Serial.print(percent*100);
    //Serial.print("%) ");
    //Serial.print(num_leds_on);
    //Serial.print(" leds should be on");
    //Serial.println();
    delay(10);
    powerbutton.check();
  }
}


void ledblink() {
  for (int t = 0; t < 5; t++) {
    delay(500);
    blackout();
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, COLORARRAY[i]);
    }
    delay(500);
    pixels.show();
  }
  delay(2000);
  cleanup();
}


void blackout() {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();
}


void cleanup() {
  for (int i = 0; i < NUMPIXELS; i++) {
    COLORARRAY[i] = pixels.Color(0,0,0);
  }
  blackout();
  should_run = false;
}


void set_colors(uint32_t color, int num_leds_on) {
  for (int i = 0; i < num_leds_on; i++) {
    pixels.setPixelColor(i, color);
    COLORARRAY[i] = color;
  }
  pixels.show();
}


void handleEvent(AceButton* /* button */, uint8_t eventType, uint8_t /* buttonState */) {
  switch (eventType) {
    case AceButton::kEventReleased:
      //digitalWrite(LED_BUILTIN, HIGH); // For visual to see if button is clicked
      //delay(100);
      //digitalWrite(LED_BUILTIN, LOW);
      
      if (!should_run) {
        should_run = true;  
      } else {
        should_run = false;  
      }
      break;
  }
}

