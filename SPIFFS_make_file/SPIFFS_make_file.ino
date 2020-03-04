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
 
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
 
  while(file){
      Serial.print("FILE: ");
      Serial.println(file.name());
      tft.setTextWrap(false);
      tft.setCursor(10, 0);
      tft.setTextColor(CYAN);
      tft.setTextSize(1);
      tft.println("Uspjesno zapisana datoteka");
      tft.setCursor(10, 30);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("FILE: ");
      tft.println(file.name());
     
  }
}
 
void loop() {}
