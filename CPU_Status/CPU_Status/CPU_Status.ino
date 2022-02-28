//--------------------------------------------------------------------
//------------------------ VIDI 312 ----------------------------------
//------------------ kod za serijsku komunikaciju --------------------
//--- napisan za potrebe članka u časopisu VIDI 312 - ožujak 2022. ---
//--------------------------------------------------------------------
//
// Kod radi s VIDI project X pločicom o kojoj možete saznati na linku
// https://vidilab.com/vidi-project-x
//
//-------------------------------------------------------------------
// napisao: Hrvoje Šomođi, Vidi - 05.02.2022.
//-------------------------------------------------------------------
//
// Nedostaju li vam niže spomenuti libraryji
// odaberite Manage Libraries sa izboranika Alti
// zatim u tražilicu upišite ILI9341
// i instalirajte ponuđenu Adafruit ILI9341 librari
// te ostale librarije koji su ponuđeni uz ovu!
//
#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include <SPI.h>

// ILI9341 TFT LCD deklaracija spajanja zaslona
#define TFT_CS   5
#define TFT_DC  21

// Stvori objekt zaslona koji zovemo TFT
Adafruit_ILI9341 TFT = Adafruit_ILI9341(TFT_CS, TFT_DC);

void setup()
{
  Serial.begin(115200); //open serial via USB to PC on default port

  TFT.begin();                   // inicijalizacuija zaslona
  TFT.setRotation(3);            // postavi orijentaciju
  TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
  TFT.setTextColor(ILI9341_BLUE, ILI9341_BLACK);
  TFT.setTextSize(5);
  TFT.print("Hi:");              // ispiši kao znak da smo spremni
}

void loop()
{
  if (Serial.available())           // provjeri dolazi li promet sa seriskog (USB) porta PC-a
  {                                 // ukoliko ima podataka 
    TFT.print(char(Serial.read())); // pročitaj serijski promet i ispiši ga na ekran 
  } else {                          // ukoliko nema podataka 
    TFT.setCursor(0, 0);            // postavi kursor na počenu poziciju ekrana
  }
}
