//--------------------------------------------------------------------
//------------------------ VIDI 310 ----------------------------------
//------------ kod za APDS 9960 kao senzor kiselosti -----------------
//-- napisan za potrebe članka u časopisu VIDI 310 - siječanj 2022. --
//--------------------------------------------------------------------
//
// Kod radi s VIDI project X pločicom o kojoj možete saznati na linku
// https://vidilab.com/vidi-project-x
//
//
//-------------------------------------------------------------------
// napisao: Hrvoje Šomođi, Vidi - 21.12.2021.
//-------------------------------------------------------------------
//
// Nedostaju li vam niže spomenuti libraryji
// odaberite Manage Libraries sa izboranika Alti
// zatim u tražilicu upišite ILI9341 i SparkFun_APDS9960
// i instalirajte ponuđenu Adafruit ILI9341 i SparkFun_APDS9960 librari
// te ostale librarije koji su ponuđene uz njih!
//
#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include <SPI.h>

#include "Wire.h"                 //Dodavanje Wire biblioteke.
#include "SparkFun_APDS9960.h"    //Dodavanje biblioteke senzora.

// ILI9341 TFT LCD deklaracija spajanja zaslona
#define TFT_CS   5
#define TFT_DC  21

#define I2C_SDA 32
#define I2C_SCL 33

SparkFun_APDS9960 apds = SparkFun_APDS9960();

uint16_t kolcinaSvjetlosti = 0;   //Varijabla za spremanje iznosa kolicine svjetlosti.
uint16_t crvenaBoja = 0;          //Varijabla za iznos kolicine crvene svjetlosti.
uint16_t zelenaBoja = 0;          //Varijabla za iznos kolicine zelene svjetlosti.
uint16_t plavaBoja = 0;           //Varijabla za iznos kolicine plave svjetlosti.

// Stvori objekt zaslona koji zovemo TFT
Adafruit_ILI9341 TFT = Adafruit_ILI9341(TFT_CS, TFT_DC);

// varijable za dimenzije zaslona
int myWidth;
int myHeight;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL, 100000);

  Serial.begin(115200);   //Inicijalizacija serijske komunikacije i postavljanje njene brzine (115200 bauda).

  TFT.begin();                   // inicijalizacija zaslona
  TFT.setRotation(3);            // postavi orijentaciju
  myWidth  = TFT.width() ;       // ekran je širok
  myHeight = TFT.height();       // ekran je visok
  TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
  TFT.setTextSize(4);
  TFT.setTextColor(ILI9341_RED); // postavljamo boju teksta

  Serial.println();     //Ispis poruke kako bismo znali da je komunikacija izmedju racunala i VIDI X-a u redu.

  Serial.println(F("**********Test ADPS-99600 Senzora – Senzor boje**********"));

  //Inicijalizacija biblioteke za senzor i provjera da li je senzor uopce spojen na Croduino.
  if ( apds.init() ) {
    Serial.println(F("Inicijalizacija biblioteke za APDS-9960 senzor je uspjesna!"));
  } else {
    Serial.println(F("Inicijalizacija biblioteke je neuspjesna. Senzor nije pronadjen. Provjeri spojeve!"));
  }

  //Ako je inicijalicija bila uspjesna, pokreni senzor za boju.
  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Senzor boje je uspjesno aktiviran. :)"));
  } else {
    Serial.println(F("Nije moguce prokrenuti senzor za boju... :("));
  }

  //Pricekajmo malo da se senzor stabilizira i kalibrira.
  delay(500);
}

void loop() {

  //Procitaj iznose svjetline, te iznose crvene, zelene i plave boje.
  //Ako nesto od toga nijie uspješno ocitano, dojavi nam grešku.
  if (  !apds.readAmbientLight(kolcinaSvjetlosti) ||
        !apds.readRedLight(crvenaBoja) ||
        !apds.readGreenLight(zelenaBoja) ||
        !apds.readBlueLight(plavaBoja) ) {
    Serial.println("Greska tijekom citanja vrijednosti sa senzora.");
  } else {
    Serial.print("Svjetlina prostora: ");   //Ispisi vrijednosti na Serial monitoru.
    Serial.print(kolcinaSvjetlosti);
    Serial.print(" Crvena boja: ");
    Serial.print(crvenaBoja);
    Serial.print(" Zelena boja: ");
    Serial.print(zelenaBoja);
    Serial.print(" Plava boja: ");
    Serial.print(plavaBoja);

    int c = map(crvenaBoja, 0, kolcinaSvjetlosti, 0, 255);
    int z = map(zelenaBoja, 0, kolcinaSvjetlosti, 0, 255);
    int p = map(plavaBoja, 0, kolcinaSvjetlosti, 0, 255);

    Serial.print(" RGB Boja ");
    Serial.print(c);
    Serial.print(" ");
    Serial.print(z);
    Serial.print(" ");
    Serial.println(p);

    TFT.fillScreen(TFT.color565(c, z, p));
    //TFT.fillScreen(TFT.color565(crvenaBoja, zelenaBoja, plavaBoja));
    if ( zelenaBoja > 25 ) {
      if ( crvenaBoja > 30 ) { //ukoliko JE
        TFT.setTextColor(ILI9341_RED); // postavljamo boju teksta
        TFT.setCursor(90, 20);
        TFT.print("KISELO");
      } else { //ukoliko NIJE
        TFT.setTextColor(ILI9341_YELLOW); // postavljamo boju teksta
        TFT.setCursor(80, 20);
        TFT.print("LUZNATO");
      }
    } else { //ukoliko NIJE 
      TFT.setTextColor(ILI9341_WHITE); // postavljamo boju teksta
      TFT.setCursor(90, 20);
      TFT.print("PRAZNO");
    }
    TFT.setTextColor(ILI9341_YELLOW); // postavljamo boju teksta
    TFT.setCursor(90, 120);
    TFT.print("VIDI X");
    TFT.setCursor(65, 152);
    TFT.print("detektor");
    TFT.setCursor(55, 184);
    TFT.print("kiselosti");
  }

  //Pricekaj jednu sekundu prije novog citanja podataka sa senzora.
  delay(1000);
}
