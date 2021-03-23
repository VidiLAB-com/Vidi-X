#include <ComponentObject.h>
#include <RangeSensor.h>
#include <SparkFun_VL53L1X.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>
#include <Wire.h>
#include "SparkFun_VL53L1X.h"
//Nemate li library, kliknite na link za instalaciju:
// http://librarymanager/All#SparkFun_VL53L1X

//fedinirajmo I2C komunikacijske pinove
#define I2C_SDA 33
#define I2C_SCL 32

//Deklarirajmo objekt senzora.
SFEVL53L1X distanceSensor;

#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include <SPI.h>
//Nemate li library, kliknite na link za instalaciju:
// http://librarymanager/All#Adafruit_ILI9341
// Obavezno kliknite na "Install All" gumb

// ILI9341 TFT LCD deklaracija spajanja zaslona
#define TFT_CS   5
#define TFT_DC  21
#define TFT_RST  4

Adafruit_ILI9341 TFT = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

int myWidth;
int myHeight;
int j = 0;
int distance;              //varijabla za udaljenost
int distance_old;          //varijabla s kojom pamtimo staru udaljenost
const int Btn_Menu = 13;   // Gumb Menu spojen je na pin 13
const int Btn_Select = 27; // Gumb Select spojen je na pin 27
int Udaljenost = 400;      //Varijabla za željenu detekciju udaljenosti

const int brojUzoraka = 10;  // broj uzoraka udaljenosti spremljene u niz, Mora biti contanta
int distValues[brojUzoraka] = { 600, 600, 600, 600, 600, 600, 600, 600, 600, 600}; // niz
float srednjaVreijdnost;     //Srednja vrijednost treba nam za specijane slućajeve

const int PinPiezo = 25;   //VIDI X zvućnik spojen je na pin 25
const int trajanje = 300;  //Trajanje note
bool arm = false;          //Varijabla za stanje alarma (ON/OFF)
bool arm_old = false;      //Varijabla za spremanje prošlog stanje alarma (ON/OFF)
const int stat = 2;        //Startus LED spojen je na pin 2
#define NOTA_C4 262 //Definiramo note
#define NOTA_C5 523

void setup(void)
{
  Wire.begin(I2C_SDA, I2C_SCL, 100000); //Inicijalizacija I2C veze
  Serial.begin(115200);                 //Inicijalizacija serijske komunikacije
  Serial.println("VL53L1X kao alarm");
  if (distanceSensor.begin() != 0) //Begin vraća 0 nakon uspješne inicijalizacije senzora
  {
    Serial.println("Senzor nije inicijaliziran. Provjerite I2C vezu. Zaustavljam program...");
    while (1); //Beskonačna petlja
  }
  Serial.println("Senzor je online!");
  TFT.begin();                   // inicijalizacuija zaslona
  TFT.setRotation(3);            // postavi orijentaciju
  myWidth  = TFT.width() ;       // ekran je širok?
  myHeight = TFT.height();       // ekran je visok?
  TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
  ledcSetup(0, 10000, 12);
  ledcAttachPin(PinPiezo, 0);
  ledcWriteTone(0, 0);
  pinMode(Btn_Select, INPUT);  //Definiramo ulazne pinove
  pinMode(Btn_Menu, INPUT);
  pinMode(stat, OUTPUT);  //Definiramo izlazne pinove
  pinMode(PinPiezo, OUTPUT);
}

void loop(void)
{
  for (int j = 0; ++j <= myWidth + 1; ) // petlja je brža uz sintaksu ugnježđene liste
  {
    distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
    while (!distanceSensor.checkForDataReady())
    {
      delay(1); //ukoliko I2C komunikacija nije stabilna povečajte delay na 50, 100 ili 500
    }
    distance = distanceSensor.getDistance(); //Vrijednost udaljenosti u milimetrima
    if (distance != 0) //ako je vrijednost različita od nule
    {
      distanceSensor.clearInterrupt();
      distanceSensor.stopRanging();
      Serial.print("Distance(mm): ");
      Serial.print(distance); //ispiši ju na serijsku konzolu
      Serial.println();
      //Serial.println(digitalRead(Btn_Menu));
    }
    TFT.setTextColor(ILI9341_BLACK); // postavljamo boju teksta u boju pozadine ekrana za brisanje starog teksta
    TFT.setCursor(0, 0);
    TFT.print("Distance(mm): ");
    TFT.print(distance_old); //Izbriši staru vrijednost
    TFT.setCursor(0, 10);
    TFT.print("Weaponized: ");
    TFT.print(arm_old); //Izbriši staru vrijednost
    distance_old = distance;  //Novo očitanu vrijednost postavi kao staru
    arm_old = arm;
    TFT.setTextColor(ILI9341_GREEN); // postavljamo boju teksta u zelenu
    TFT.setCursor(0, 0);
    TFT.print("Distance(mm): "); //Ispiši novu vrijednost
    TFT.print(distance);
    TFT.setCursor(0, 10);
    TFT.print("Weaponized: ");
    TFT.print(arm);
    TFT.drawFastVLine( j, 0, myHeight, ILI9341_BLACK);
    // nacrtaj vrijednost na ekranu
    TFT.drawPixel(j, map(distance, 0, 4095, myHeight - 1, 1), ILI9341_YELLOW);
    // nacrtaj srednju vrijednost zadnjih 10 mjerenja na ekranu
    TFT.drawPixel(j, map(srednjaVreijdnost, 0, 4095, myHeight - 1, 1), ILI9341_GREEN);
    for (int o = 0; o < brojUzoraka; o = o + 1) { // Premještamo niz kako bi napravili mjesta za novu vrijednost
      distValues[o] = distValues[o + 1];
    }
    distValues[brojUzoraka - 1] = distance;     // Spremi vrijednost senzora na zadnje mjesto niza
    srednjaVreijdnost = 0;
    for (int o = 0; o < brojUzoraka; o = o + 1) { // Pripremamo varijablu za računanje aritmetičke sredine
      srednjaVreijdnost = srednjaVreijdnost + distValues[o];
      //TFT.print(" srednjaVreijdnost=");
      //TFT.print(srednjaVreijdnost);
      //TFT.print(" distValues[o]=");
      //TFT.print(distValues[o]);
      //    TFT.print(" [o]=");
      //TFT.print(o);
    }
    srednjaVreijdnost = srednjaVreijdnost / brojUzoraka; // računamo artitmetičku sredinu
    if ( !digitalRead(Btn_Menu) ) { //ukoliko je pritisnut gumb
      arm = !arm;                   //promijeni vrijednost logičke varijable - aktiviraj/deaktiviraj alarm
      ledcWriteTone (0, NOTA_C5);   //odsviraj zvuk stisnutog gumba
      delay(trajanje);
      ledcWriteTone(0, 0); //utišaj zvučnik
      delay(500); //Kada smo pritisnuli gumb trebamo odgodu
      Serial.print("MENU je stisnut");
      Serial.println();
      Serial.print("Weaponized: ");
      Serial.print(arm);
    }

    if ( !digitalRead(Btn_Select) ) { //ukoliko je pritisnut gumb
      Udaljenost = distance; //definiraj udaljenost koju želimo pratiti
      ledcWriteTone (0, NOTA_C5); //odsviraj zvuk stisnutog gumba
      delay(trajanje);
      ledcWriteTone(0, 0); //utišaj zvučnik
      delay(500); //Kada smo pritisnuli gumb trebamo odgodu
      Serial.print("Stisnut je SELECT");
      Serial.println();
      Serial.print("Udaljenost: ");
      Serial.print(Udaljenost);
    }

    if ( arm ) { //ukoliko je alarm aktivan
      if ( distance < ( Udaljenost - 300 ) ) { //Ukoliko se udaljenost smanjila za 30 centimatara
        TFT.drawPixel(j, map(distance, 0, 4095, myHeight - 1, 1), ILI9341_RED);             // iscrtaj graf crvene boje,
        TFT.drawPixel(j, map(srednjaVreijdnost, 0, 4095, myHeight - 1, 1), ILI9341_PURPLE); // a srednju vrijednost ružićaste boje
        ledcWriteTone (0, NOTA_C4); // Sviraj zvuk aktiviranog alarma
        delay(trajanje);
        ledcWriteTone(0, 0);
        digitalWrite(stat, HIGH); // Upali Status LED
        //Debug vrijednosti na serijsku konzolu
        Serial.println();
        Serial.print("Udaljenost > distance -300");
        Serial.println();
        Serial.print(Udaljenost);
        Serial.print(" > ");
        Serial.print(distance - 300);
        Serial.println();
      } else {  //Ukoliko se udaljenost NIJE smanjila za 30 centimatara
        digitalWrite(stat, LOW); //Ugasi Status LED
      }
    } else { //ukoliko NIJE alarm aktivan
      digitalWrite(stat, LOW); //Ugasi Status LED
    }

  }
}
