#include <Arduino.h>
#include <FastLED.h>
#include <driver/adc.h>
#include <complex.h>
#include "params.h"
#include <TFT_eSPI.h>

#include <SPI.h>
#define MIC 14 // Pin mikrofona

// How many leds in your strip?
#define NUM_LEDS 142    // Broj LEDica na traci
#define DATA_PIN 15     // Pin na koji spajamo podatkovnu liniju LED trake
#define BOJA 2800       // Količibna boje može biti vrijednost između 4095 i 0
#define FAKTOR 10       // Faktor množenja boje za narednu ledicu, može biti vrijednost od -10 do 10, ali i 0.01 te druge vrijednosti
#define MAXSVJETLINA 95 // 255 je najsvjetlije
#define MINSVJETLINA 35 // 0 je ugašeno
#define MINBOJA 0       // Za puni spektar boja MIN i MAX se postavljaju od 0 do 255
#define MAXBOJA 255     // 0-10 - zelena; -30 - narančasta; 

// Define the array of leds
CRGB leds[NUM_LEDS];
#define FREQ2IND (SAMPLES * 1.0 / MAX_FREQ)

#define BUTTON_1            35 // UP Down
#define BUTTON_2            34 // Volume
#define BUTTON_3            27 // UP Sense
#define BUTTON_4            13 // Down Sense

int SENSITIVITY = 15;

TFT_eSPI display = TFT_eSPI(); // Invoke custom library

unsigned long chrono, chrono1;
unsigned long sampling_period_us;
byte peak[SAMPLES] = {0};
float complex data[SAMPLES];
int sound[SAMPLES];
float MULT = MAX_FREQ * 1000.0 / SAMPLES;
unsigned int P2P = 0;
int LOG2SAMPLE = log(SAMPLES) / log(2);
#include "functions.h"
#define MODE 19
#define MAXMODES 5

void setup() {
  //Serial.begin(115200);
  Serial.begin(57600);
  pinMode(BUTTON_1, INPUT_PULLUP);
    pinMode(BUTTON_2, INPUT_PULLUP);
      pinMode(BUTTON_3, INPUT_PULLUP);
        pinMode(BUTTON_4, INPUT_PULLUP);
  Serial.println("1");
  display.init();
  display.fillScreen(TFT_BLACK);
  display.setRotation(3);
  display.setTextColor(TFT_BLUE);
  display.setTextSize(2);
  display.drawString("LIGHT", 0, 10, 4);
  display.drawString("SHOW", 68, 60, 4);
  delay(2000);
  sampling_period_us = round(1000ul * (1.0 / MAX_FREQ));
  chrono1 = millis();
  Serial.println("2");
  LEDS.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
  LEDS.setBrightness(84);
  Serial.println("3");
}

void loop() {


  if (digitalRead(BUTTON_3) == LOW ) { SENSITIVITY--; Serial.println(SENSITIVITY); delay(100); }
  if (digitalRead(BUTTON_4) == LOW ) { SENSITIVITY++; Serial.println(SENSITIVITY); delay(100); }

  
  static int modes = 0;
  bool changeMode = false;
  // Push the button to change modes
  if (millis() - chrono1 > 1000ul) {
    if (digitalRead(BUTTON_1) == HIGH || digitalRead(BUTTON_2) == HIGH) {
      delay(30);
      modes = (modes + 1) % MAXMODES;
      //    Serial.printf("Mode %d\n", modes);
      changeMode = true;
      chrono1 = millis();
    }
  }

  switch (modes) {
    case 0: // Display spectrum
      acquireSound();
      displaySpectrum();
      break;
    case 1: // Display spectrum
      acquireSound();
      displaySpectrum2();
      break;
    case 2: // Display amplitude with bars
      displayAmplitudeBars();
      break;
    case 3: // Display envelope
      displayEnvelope(changeMode);
      break;
    case 4: // Display running amplitude
      acquireSound();
      displaySpectrum4();
      break;
    default:
      break;
  }
}
