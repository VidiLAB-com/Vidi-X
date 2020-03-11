#include <SPIFFS.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

/* Display */
#define TFT_DC 21
#define TFT_CS 5

/* color definition */
#define WHITE    0xFFFF
#define CYAN     0x07FF
#define BLACK    0x0000

/* Display library*/
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
 
void setup() {
  tft.begin();
  tft.fillScreen(BLACK);
/*set screen rotation*/
  tft.setRotation(3);
  Serial.begin(115200);
 
  if (!SPIFFS.begin(true)) {
    Serial.println("Greška datotečnog sustava");
    return;
  }
 
  File file = SPIFFS.open("/VIDI.txt", FILE_WRITE);
 
  if (!file) {
    Serial.println("Greska prilikom kreiranja datoteke");
    return;
  }
  if (file.print("VIDI")) {
    Serial.println("Datoteka uspjesno zapisana");
    tft.setCursor(10, 0);
    tft.setTextColor(CYAN);
    tft.setTextSize(1);
    tft.println("Datoteka uspjesno zapisana");
  } else {
    Serial.println("Greska prilikom zapisa");
    tft.setCursor(10, 0);
    tft.setTextColor(CYAN);
    tft.setTextSize(1);
    tft.println("Greska prilikom zapisa");
  }
 
  file.close();
}
 
void loop() {}
