/*
Ray tracing za ILI9341 TFT LCD display Vidi X-a
raditi će i na običnom Arduinu, zatim na "blue pill" STM32 pločicama i ESP8266 NodeMCU V1
Pogledajte niže navedene komentare za spajanje ukoliko želite testirati kod na drugm pločicama
radi usporedbe brzine.
*/

#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include <SPI.h>

// ILI9341 TFT LCD deklaracija spajanja zaslona
#define TFT_CS   5
#define TFT_DC  21
#define TFT_RST  4

// Stvori objekt zaslona koji zovemo TFT
Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

#define RGBTO565(_r, _g, _b) ((((_r) & B11111000)<<8) | (((_g) & B11111100)<<3) | ((_b) >>3))
#include "raytracer.h"

void setup() {
  Serial.begin(115200);
//Serial.begin(9600); 
  Serial.println("ILI9341 raytracing"); 
 
  display.begin();
  display.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  display.setTextSize(1);
  
  // prikaži diagnostičke podatke (nje obavezno, no može pomoći u debugiranju koda)
  uint8_t x = display.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = display.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = display.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = display.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = display.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
  Serial.print("ScreenWidth:  "); Serial.println(display.width()); 
  Serial.print("ScreenHeight: "); Serial.println(display.height()); 

  display.setRotation(3);
  display.fillScreen(0);
  
  unsigned long t = millis();
  doRaytrace(1,320,240,1); // full 320x240 jedan uzorak
  doRaytrace(1,320,240,4); // brzi prikaz
  doRaytrace(1,320,240,2); // spori prikaz
  doRaytrace(1,160,120,2); // četvrtina ekrana
  doRaytrace(8);  // visokoa kvaliteta 320x240 slike uz antialiasing s 8 uzoraka
  Serial.println(F("Done!"));
  Serial.print(F("Time = "));
  Serial.print((millis() - t)/1000);
  Serial.println(F(" seconds"));
}

void loop(void) {
}
