//--------------------------------------------------------------------
//------------------------ VIDI 300 ----------------------------------
//------------------- kod za radionicu pametni vrt -------------------
//--- napisan za potrebe članka u časopisu VIDI 300 - ožujak 2021. ---
//--------------------------------------------------------------------
//
// Kod radi s VIDI project X pločicom o kojoj možete saznati na linku
// https://vidilab.com/vidi-project-x
//-------------------------------------------------------------------
// napisao: Hrvoje Šomođi, Vidi - 25.2.2021.
//-------------------------------------------------------------------
// Nedostaju li vam niže spomenuti libraryji
// odaberite Manage Libraries sa izboranika Alti
// zatim u tražilicu upišite ILI9341
// i instalirajte ponuđenu Adafruit ILI9341 librari
// te ostale librarije koji su ponuđeni uz ovu!
// Zatim u tražilicu upišite FastLED
// Zatim u tražilicu upišite Adafruit_BME680

#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#include <FastLED.h>

#define LED_PIN     22
#define NUM_LEDS    7

CRGB leds[NUM_LEDS];

// ILI9341 TFT LCD deklaracija spajanja zaslona
#define TFT_CS   5
#define TFT_DC  21

#define I2C_SDA 33
#define I2C_SCL 32

#define SEALEVELPRESSURE_HPA (1018.25)

Adafruit_BME680 bme; // I2C
//Adafruit_BME680 bme(BME_CS); // hardware SPI
//Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

// Stvori objekt zaslona koji zovemo TFT
Adafruit_ILI9341 TFT = Adafruit_ILI9341(TFT_CS, TFT_DC);

const int analogInPinTla = 2;         //Senzor Vlage tla
int SuhoTlo = 4095;                   // Smanjiti vrijednost ukoliko želimo ćešće zaljevanje zemlje
const int analogInPinSvjetlosti = 12; // Svjetlost
const int analogInPinTempTla = 13;    // Temperatura Tla

int debug = 0;              // Pokreni testni mod
int konzola = 1;            // Ispisuj na Serial
const int VodenaPumpa = 14; // Driver pumpe
const int Btn_Up = 35;      // Ovo nam za sada neće trebati
const int Noc = 4000;

// varijable za dimenzije zaslona
int myWidth;
int myHeight;

// varijable za očitanja senzora
int BME680temp;
int BME680pres;
int BME680humi;
int BME680gas;
int BME680alt;
float Ttemp;

// varijable za spremanje starih očitanja senzora
int old_BME680temp;
int old_BME680pres;
int old_BME680humi;
int old_BME680gas;
int old_BME680alt;
float old_Ttemp;

void PaliSvjetlo() { // funkcija za paljenje LED-ica
  FastLED.clear();
  for (int o = 0; o < NUM_LEDS; o = o + 1) {
    leds[o] = CRGB(155, 0, 155);
    FastLED.show();
  }
}

void GasiSvjetlo() { // funkcija za gašenje LED-ica
  FastLED.clear();
  for (int o = 0; o < NUM_LEDS; o = o + 1) {
    leds[o] = CRGB(0, 0, 0);
    FastLED.show();
  }
}

void senzorBME680()
{
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }

  old_BME680temp = BME680temp;
  old_BME680pres = BME680pres;
  old_BME680humi = BME680humi;
  old_BME680gas = BME680gas;
  old_BME680alt = BME680alt;
  old_Ttemp = Ttemp;

  BME680temp = bme.temperature;
  BME680pres = bme.pressure;
  BME680humi = bme.humidity;
  BME680gas = bme.gas_resistance;
  BME680alt = bme.readAltitude(SEALEVELPRESSURE_HPA);

  Ttemp = map(analogRead(analogInPinTempTla), 0, 4095, -55, 125);

  if ( konzola ) {
    Serial.print("Temperature = ");
    Serial.print(BME680temp);
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(BME680pres / 100.0);
    Serial.println(" hPa");

    Serial.print("Humidity = ");
    Serial.print(BME680humi);
    Serial.println(" %");

    Serial.print("Gas = ");
    Serial.print(BME680gas / 1000.0);
    Serial.println(" KOhms");

    Serial.print("Approx. Altitude = ");
    Serial.print(BME680alt);
    Serial.println(" m");

    Serial.println();
  }

  TFT.setTextColor(ILI9341_BLACK);
  TFT.setCursor(150, 0);
  TFT.print("Temperature = ");
  TFT.print(old_BME680temp);
  TFT.println(" *C");
  TFT.setCursor(150, 10);
  TFT.print("Pressure = ");
  TFT.print(old_BME680pres / 100.0);
  TFT.println(" hPa");
  TFT.setCursor(150, 20);
  TFT.print("Humidity = ");
  TFT.print(old_BME680humi);
  TFT.println(" %");
  TFT.setCursor(150, 30);
  TFT.print("Gas = ");
  TFT.print(old_BME680gas / 1000.0);
  TFT.println(" KOhms");
  TFT.setCursor(150, 40);
  TFT.print("Approx. Altitude = ");
  TFT.print(old_BME680alt);
  TFT.println(" m");
  TFT.setCursor(150, 50);
  TFT.print("Temperatura tla = ");
  TFT.print(old_Ttemp);
  TFT.println(" *C");

  TFT.setTextColor(ILI9341_YELLOW);
  TFT.setCursor(150, 0);
  TFT.print("Temperature = ");
  TFT.print(BME680temp);
  TFT.println(" *C");
  TFT.setCursor(150, 10);
  TFT.print("Pressure = ");
  TFT.print(BME680pres / 100.0);
  TFT.println(" hPa");
  TFT.setCursor(150, 20);
  TFT.print("Humidity = ");
  TFT.print(BME680humi);
  TFT.println(" %");
  TFT.setCursor(150, 30);
  TFT.print("Gas = ");
  TFT.print(BME680gas / 1000.0);
  TFT.println(" KOhms");
  TFT.setCursor(150, 40);
  TFT.print("Approx. Altitude = ");
  TFT.print(BME680alt);
  TFT.println(" m");
  TFT.setCursor(150, 50);
  TFT.print("Temperatura tla = ");
  TFT.print(Ttemp);
  TFT.println(" *C");


}
void setup()
{
  Serial.begin(115200);

  pinMode(VodenaPumpa, OUTPUT);
  digitalWrite(VodenaPumpa, LOW);
  pinMode(Btn_Up, INPUT);

  // pri bootanju postavi sve aktuatore na LOW
  digitalWrite(analogInPinTla, LOW);
  digitalWrite(analogInPinSvjetlosti, LOW);

  Wire.begin(I2C_SDA, I2C_SCL, 100000);

  while (!Serial);
  Serial.println(F("BME680 test"));

  if (debug == 1) {
    if (!bme.begin()) {
      Serial.println("Could not find a valid BME680 sensor, check wiring!");
      while (1);
    }
  } else {
    bme.begin();
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  TFT.begin();                   // inicijalizacuija zaslona
  TFT.setRotation(3);            // postavi orijentaciju
  myWidth  = TFT.width() ;       // ekran je širok
  myHeight = TFT.height();       // ekran je visok
  TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
  TFT.setTextSize(1);
  TFT.setTextColor(ILI9341_YELLOW);
  TFT.println("Svjetlost  Tlo    Tlo");
  TFT.println("          Vlaga  Temp.");

  TFT.setTextSize(3);
  TFT.setCursor(150, 110);
  TFT.print("VIDI X ");
  TFT.setCursor(150, 140);
  TFT.println("PAMETNI");
  TFT.setCursor(150, 170);
  TFT.println("VRT !");
  TFT.setTextSize(1);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
}

void loop()
{
  if (debug == 1) {
    if ( digitalRead(Btn_Up) == HIGH ) {
      GasiSvjetlo();
      if ( konzola ) {
        Serial.println("Dan je - GASIM SVJETLO");
      }
    } else {
      PaliSvjetlo();
      if ( konzola ) {
        Serial.println("Noc je - PALIM SVJETLO ");
      }
    }

    if ( digitalRead(Btn_Up) == HIGH ) {
      digitalWrite(VodenaPumpa, HIGH);
      if ( konzola ) {
        Serial.println("VodenaPumpa HIGH - PALIM PUMPU ");
      }
    } else
    {
      digitalWrite(VodenaPumpa, LOW);
      if ( konzola ) {
        Serial.println("VodenaPumpa LOW - GASIM PUMPU");
      }
    }
  } else {
    if (analogRead(analogInPinTla) < SuhoTlo) {
      digitalWrite(VodenaPumpa, LOW);
      if ( konzola ) {
        Serial.println("VodenaPumpa LOW - GASIM PUMPU");
      }
    } else {
      digitalWrite(VodenaPumpa, HIGH);
      if ( konzola ) {
        Serial.println("VodenaPumpa HIGH - PALIM PUMPU ");
      }
    }

    if (analogRead(analogInPinSvjetlosti) < 2000) {
      Serial.println(analogRead(analogInPinSvjetlosti));
      GasiSvjetlo();
      if ( konzola ) {
        Serial.println("Dan je - GASIM SVJETLO");
      }
    } else {
      Serial.println(Noc);
      PaliSvjetlo();
      if ( konzola ) {
        Serial.println("Noc je - PALIM SVJETLO ");
      }
    }
  }
  TFT.fillRect( 15, 25, 25, myHeight - map(analogRead(analogInPinSvjetlosti), 0, 4095, myHeight - 10, 1) - 11, ILI9341_BLACK);
  TFT.fillRect( 15, myHeight - map(analogRead(analogInPinSvjetlosti), 0, 4095, myHeight - 25, 1), 25, myHeight - 10, ILI9341_RED);

  TFT.fillRect( 62, 25, 25, myHeight - map(analogRead(analogInPinTla), 0, 4095, myHeight - 10, 1) - 11, ILI9341_BLACK);
  TFT.fillRect( 62, myHeight - map(analogRead(analogInPinTla), 0, 4095, myHeight - 25, 1), 25, myHeight - 10, ILI9341_YELLOW);

  TFT.fillRect( 97, 25, 25, myHeight - map(analogRead(analogInPinTempTla), 0, 4095, myHeight - 10, 1) - 11, ILI9341_BLACK);
  TFT.fillRect( 97, myHeight - map(analogRead(analogInPinTempTla), 0, 4095, myHeight - 25, 1), 25, myHeight - 10, ILI9341_GREEN);

  senzorBME680();
  delay(2000);
}
