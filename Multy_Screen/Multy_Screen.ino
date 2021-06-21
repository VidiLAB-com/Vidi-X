//--------------------------------------------------------------------
//------------------------ VIDI 304 ----------------------------------
//------------------ kod za tri monitora -----------------------------
//--- napisan za potrebe članka u časopisu VIDI 304 - srpanj 2021. ---
//--------------------------------------------------------------------
//
// Kod radi s VIDI project X pločicom o kojoj možete saznati na linku
// https://vidilab.com/vidi-project-x
//
//-------------------------------------------------------------------
// napisao: Hrvoje Šomođi, Vidi - 21.6.2021.
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
#define TFT_CS1  5
#define TFT_CS3 13
#define TFT_CS2 19
#define TFT_DC  21

// Stvori objekte zaslona koje zovemo TFTx
Adafruit_ILI9341 TFT1 = Adafruit_ILI9341(TFT_CS1, TFT_DC);
Adafruit_ILI9341 TFT2 = Adafruit_ILI9341(TFT_CS2, TFT_DC);
Adafruit_ILI9341 TFT3 = Adafruit_ILI9341(TFT_CS3, TFT_DC);

// Analogni pin 14 spojen je na mikrofon sa mikrodfonskim predpojačalom
const int analogInPin1 = 14;   // Analogni input pin mikrofona
const int analogInPin2 = 26;   // Analogni input pin senzora temperature
const int analogInPin3 = 2;   // Analogni input pin statul LED-ice koja može pozlužiti i kao detektor svijetlosti

// varijable za dimenzije zaslona
int myWidth;
int myHeight;

void setup()
{
  pinMode(analogInPin1, INPUT);   // Postavi pin senzora kao INPUT
  pinMode(analogInPin2, INPUT);   // Postavi pin senzora kao INPUT
  pinMode(analogInPin3, INPUT);   // Postavi pin senzora kao INPUT
  TFT1.begin();                   // inicijalizacija zaslona br 1
  TFT2.begin();                   // inicijalizacija zaslona br 2
  TFT3.begin();                   // inicijalizacija zaslona br 3
  TFT1.setRotation(3);            // postavi orijentaciju
  TFT2.setRotation(1);            // postavi orijentaciju
  TFT3.setRotation(1);            // postavi orijentaciju
  myWidth  = TFT1.width() ;       // ekran je širok
  myHeight = TFT1.height();       // ekran je visok
  TFT1.fillScreen(ILI9341_BLACK); // obojaj zaslon 1 u crno
  TFT2.fillScreen(ILI9341_BLACK); // obojaj zaslon 2 u crno
  TFT3.fillScreen(ILI9341_BLACK); // obojaj zaslon 3 u crno
  TFT1.setTextSize(1);
  TFT1.setTextColor(ILI9341_RED);
  TFT1.println("Monitor 1 - Analog PIN 14 - MIC");
  TFT2.setTextSize(1);
  TFT2.setTextColor(ILI9341_GREEN);
  TFT2.println("Monitor 2 - Analog PIN 26 - Senzor temperature");
  TFT3.setTextSize(1);
  TFT3.setTextColor(ILI9341_BLUE);
  TFT3.println("Monitor 3 - Analog PIN 2 - Detektor svijetlosti");
}

void loop()
{
  for (int j = 0; j++ <= myWidth + 1; ) // petlja je brža uz sintaksu ugnježđene liste
  { // sporija sintaksa petlje izgleda // for(int j = 0; j <= myWidth + 1; j = j + 1)
    TFT1.drawFastVLine( j, 10, myHeight, ILI9341_BLACK);
    TFT1.drawFastVLine( j, map(analogRead(analogInPin1), 0, 4095, myHeight - 1, 1), myHeight, ILI9341_RED);
    TFT1.drawPixel(j, map(analogRead(analogInPin1), 0, 4095, myHeight - 1, 1), ILI9341_PURPLE);
    TFT2.drawFastVLine( j, 10, myHeight, ILI9341_BLACK);
    TFT2.drawFastVLine( j, map(analogRead(analogInPin2), 0, 4095, myHeight - 1, 1), myHeight, ILI9341_DARKGREEN);
    TFT2.drawPixel(j, map(analogRead(analogInPin2), 0, 4095, myHeight - 1, 1), ILI9341_GREEN);
    TFT3.drawFastVLine( j, 10, myHeight, ILI9341_BLACK);
    TFT3.drawFastVLine( j, map(analogRead(analogInPin3), 0, 4095, myHeight - 1, 1), myHeight, ILI9341_CYAN);
    TFT3.drawPixel(j, map(analogRead(analogInPin3), 0, 4095, myHeight - 1, 1), ILI9341_BLUE);
  }
}
