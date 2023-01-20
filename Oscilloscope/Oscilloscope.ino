//--------------------------------------------------------------------
//------------------------ VIDI 316 ----------------------------------
//------------------ kod za Oscilloscope -----------------------------
//--- napisan za potrebe članka u časopisu VIDI 316 - srpanj 2022. ---
//--------------------------------------------------------------------
//
// Kod radi s VIDI project X pločicom o kojoj možete saznati na linku
// https://vidilab.com/vidi-project-x
//  
// 
//-------------------------------------------------------------------
// napisao: Hrvoje Šomođi, Vidi - 12.2.2020.
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
#define TFT_RST  4

// Stvori objekt zaslona koji zovemo TFT
Adafruit_ILI9341 TFT = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Analogni pin 14 spojen je na mikrofon sa mikrodfonskim predpojačalom
const int analogInPin = 35;   // Analogni input pin

// varijable za dimenzije zaslona
int myWidth;
int myHeight;

void setup() 
{
  pinMode(analogInPin, INPUT);   // Postavi pin senzora kao INPUT
    pinMode(analogInPin-1, INPUT);   // Postavi pin senzora kao INPUT
  TFT.begin();                   // inicijalizacuija zaslona
  TFT.setRotation(3);            // postavi orijentaciju
  myWidth  = TFT.width() ;       // ekran je širok
  myHeight = TFT.height();       // ekran je visok
  TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
}

void loop()
{
  for(int j = 0; j++ <= myWidth + 1; ) // petlja je brža uz sintaksu ugnježđene liste
  { // sporija sintaksa petlje izgleda // for(int j = 0; j <= myWidth + 1; j = j + 1)
    TFT.drawFastVLine( j, 0, myHeight, ILI9341_BLACK);
    // Pixel crtamo ugnježđivanjem listi od kojih je
    // analogRead(analogInPin)
    // najdulblje ugnježđena lista koja je element
    // map(analogRead(analogInPin), 0, 4095, myHeight - 1, 1)
    // liste, te je sve zajeno ugnježđeno u 
    TFT.drawPixel(j, map(analogRead(analogInPin), 0, 4095, myHeight - 1, 1), ILI9341_YELLOW);
        TFT.drawPixel(j, map(analogRead(analogInPin-1), 0, 4095, myHeight - 1, 1), ILI9341_GREEN);
    // Kada bismo za funkciju
    // analogRead(analogInPin)
    // odredili varijablu poput
    // int n = analogRead(analogInPin)
    // pa zatim taj n uvrstili u narednu funkciju kao
    // map(n, 0, 4095, myHeight - 1, 1)
    // program bi se izvodio duplo sporije
  }
}
