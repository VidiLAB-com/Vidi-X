
#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include <SPI.h>

int gpio = 32;
int _freq = 250;
int _channel = 0;
int _resolution = 8;

// ILI9341 TFT LCD deklaracija spajanja zaslona
#define TFT_CS   5
#define TFT_DC  21

// Stvori objekt zaslona koji zovemo TFT
Adafruit_ILI9341 TFT = Adafruit_ILI9341(TFT_CS, TFT_DC);

// varijable za dimenzije zaslona
int myWidth;
int myHeight;

void ekran() {
  Serial.println(_freq);
  TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
  TFT.setTextColor(ILI9341_RED);
  TFT.setTextSize(3);
  TFT.setCursor(100, 60);
  TFT.print("VIDI X ");
  TFT.setCursor(80, 90);
  TFT.setTextColor(ILI9341_GREEN);
  TFT.println("GENERATOR");
  TFT.setCursor(40, 120);
  TFT.println("FREKVENCIJE !");
  TFT.setTextColor(ILI9341_YELLOW);
  TFT.setCursor(40, 170);
  TFT.print("Freq: ");
  TFT.setTextSize(4);
  TFT.setCursor(140, 170);
  TFT.print(_freq);
}

void setup() {
  Serial.begin(115200);
  ledcSetup(_channel, _freq, _resolution);
  ledcAttachPin(gpio, _channel);
  TFT.begin();                   // inicijalizacuija zaslona
  TFT.setRotation(3);            // postavi orijentaciju
  TFT.setTextSize(3);
  myWidth  = TFT.width() ;       // ekran je širok
  myHeight = TFT.height();       // ekran je visok
}

void loop() {
  for (_freq = 250; _freq < 10000; _freq = _freq + 250) {
    ekran();
    ledcWriteTone(_channel, _freq);
    delay(2000);
  }
}
