//--------------------------------------------------------------------
//------------------------ VIDI 299 ----------------------------------
//------------------ kod za MQ-7 CO senzor ---------------------------
//--- napisan za potrebe članka u časopisu VIDI 299 - veljača 2021. ---
//--------------------------------------------------------------------
//
// Kod radi s VIDI project X pločicom o kojoj možete saznati na linku
// https://vidilab.com/vidi-project-x
//
//
//-------------------------------------------------------------------
// napisao: Hrvoje Šomođi, Vidi - 18.1.2021.
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
const int analogInPin = 14;  // Digitalni input pin
const int analogInPin2 = 13; // Analogni input pin
const int Alarm = 2;         // Pin plave ledice

// varijable za dimenzije zaslona
int myWidth;
int myHeight;
int j = 0;             // inicijaliziramo varijeble koje će nam trebati

int val = 0;
int valMin = 4096;     // ove varijeble postavljemo na besmisleno velike vrijednosti
int valMax = -4096;    // ..koje nam softver neće javiti

void dijeljenje( int n) {
  int var = n % 3; // računamo ostatak dijeljenja s 3 kako bi mogli rotirati tri boje
  switch (var) {   // u svrhu smanjivanja "burn in" efekta LCD ekrana
    case 0:
      // statements
      TFT.setTextColor(ILI9341_RED);
      break;
    case 1:
      // statements
      TFT.setTextColor(ILI9341_GREEN);
      break;
    default:
      // statements
      TFT.setTextColor(ILI9341_BLUE);
      break;
  }
}

void setup()
{ Serial.begin(115200);
  pinMode(analogInPin, INPUT);   // Postavi pin senzora kao INPUT
  pinMode(analogInPin2, INPUT);  // Postavi pin senzora kao INPUT
  pinMode(Alarm, OUTPUT);        // Postavi pin Alarma kao OUTPUT
  TFT.begin();                   // inicijalizacuija zaslona
  TFT.setRotation(3);            // postavi orijentaciju
  myWidth  = TFT.width() ;       // ekran je širok
  myHeight = TFT.height();       // ekran je visok
  TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
  digitalWrite(Alarm, LOW); 
}

void loop()
{
  for (int j = 0; j++ <= myWidth + 1; ) // petlja je brža uz sintaksu ugnježđene liste
  {
    val = analogRead(analogInPin2);       // Spremi vrijednost senzora u varijablu

    if (valMin > val) { // Ukoliko je dosegnut novi minimum
      valMin = val;     // postavi DeltaMin vrijednost
    }
    if (valMax < val) {  // Ukoliko je dosegnut novi maksimum
      valMax = val;      // postavi DeltaMax vrijednost
    }

    TFT.setTextColor(ILI9341_GREEN);
    dijeljenje(j); // postavljamo boju teksta
    TFT.fillRect(0, 0, 320, 30, ILI9341_BLACK); //Brišemo stari tekst sa ekrana
    TFT.setCursor(0, 0);
    TFT.print("DeltaMin = ");
    TFT.print(valMin); // ispisujemo na ekran
    TFT.setTextColor(ILI9341_RED);
    dijeljenje(j + 1); // postavljamo boju +1
    TFT.print(" DeltaMax = ");
    TFT.print(valMax); // ispisujemo na ekran
    TFT.setTextColor(ILI9341_BLUE);
    dijeljenje(j + 2); // postavljamo boju +2
    TFT.print(" Analog = ");
    TFT.print(val); // ispisujemo na ekran
    TFT.print(" - ");
    TFT.print(analogRead(analogInPin2)); 

    TFT.drawFastVLine( j, 31, myHeight - 1, ILI9341_BLACK);   // Crnim linijama pripremiti će mo  
    TFT.drawFastVLine( j+1, 31, myHeight - 1, ILI9341_BLACK); // ekran za nova očitanja
    TFT.drawFastVLine( j+2, 31, myHeight - 1, ILI9341_BLACK); // Potrebno nam je
    TFT.drawFastVLine( j+3, 31, myHeight - 1, ILI9341_BLACK); // nekoliko linija za rednom
    
    Serial.println(val);
    TFT.drawPixel(j, map(val, 0, 4095, myHeight - 1, 31), ILI9341_YELLOW);   // Crtamo krivulju temperature od 0 do 4095
    TFT.drawPixel(j, map(val, 1000, 2000, myHeight - 1, 31), ILI9341_GREEN); // Crtamo izraženiju krivulju temperature
    if (val > 2048) {                               // Prijeđu li vrijednosti postavljeno očitanje
      digitalWrite(Alarm, HIGH);                    // Upali plavu LE Diodu kao znak alarama
      Serial.println("Warning: Limit exceeded!!!"); // Ispiši poruku na serijsku konzolu
      TFT.drawPixel(j, map(val, 0, 4095, myHeight - 1, 31), ILI9341_RED); // Krivulju iscrtaj crvenom bojom
    }
    delay(5000); // Pričekaj 5 sekundi
  }
}
