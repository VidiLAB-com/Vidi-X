//--------------------------------------------------------------------
//------------------------ VIDI 302 ----------------------------------
//- kod VIDI X i laserski senzor udaljenosti u ulozi pametnog robota -
//--- napisan za potrebe članka u časopisu VIDI 302 - svibanj 2021. --
//--------------------------------------------------------------------
//
// Kod radi s VIDI project X pločicom o kojoj možete saznati na linku
// https://vidilab.com/vidi-project-x
//
//-------------------------------------------------------------------
// napisao: Hrvoje Šomođi, Vidi - 16.4.2021.
//-------------------------------------------------------------------
//
// Nedostaju li vam niže spomenuti libraryji
// instalirajte ih s priloženih linkova
// za instalaciju biblioteka
// http://librarymanager/All#SparkFun_VL53L1X
// http://librarymanager/All#Adafruit_ILI9341
//

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

Adafruit_ILI9341 TFT = Adafruit_ILI9341(TFT_CS, TFT_DC);

int myWidth;
int myHeight;
int j = 0;
int distance;              //varijabla za udaljenost
int distance_old;          //varijabla s kojom pamtimo staru udaljenost

int Udaljenost = 400;      //Varijabla za željenu detekciju udaljenosti

const int brojUzoraka = 10;  // broj uzoraka udaljenosti spremljene u niz, Mora biti contanta
int distValues[brojUzoraka] = { 600, 600, 600, 600, 600, 600, 600, 600, 600, 600}; // niz
float srednjaVreijdnost;     //Srednja vrijednost treba nam za specijane slućajeve

const int brojMjerenja = 16;  // broj uzoraka udaljenosti spremljene u niz, Mora biti contanta
int circValues[brojMjerenja] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // niz


const int PinPiezo = 25;   //VIDI X zvućnik spojen je na pin 25
const int trajanje = 300;  //Trajanje note

// Desni Motor
const int IN1 = 14;
const int IN2 = 13;

// Lijevi Motor
const int IN3 = 22;
const int IN4 = 12;

// Treći motor
const int IN5 = 27;
const int IN6 = 2;

#define NOTA_C4 262 //Definiramo note
#define NOTA_C5 523

int najdalje;
int element;

// Ovo je varijabla koju ćete obvezno morati prilagoditi vašem vozilu
// a ovisi o vrsti motora kao i o napajnju tih motora
// Ukoliko je napajanje slabije (potrošene baterije), varijablu će trebati povećati
// ili treba odabrati kvalitenije napajanje motora
// Treba odrediti "vrijednost" koja odgovara vremenu potrebnom za okretanje šesnajstine kruga
// Varijabla "vrijednost" će se koristiti barem dok robot ne dobije Kompas za orijentaciju
int vrijednost = 730; 

// idealno bi bilo orijebntaciju mjeriti kompasom
void Motor_Init() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(IN5, OUTPUT);
  pinMode(IN6, OUTPUT);
}

// Test motora korištenjem For petlje
void Test(int Duration) {
  // Ostani u For petlji 100 ciklusa
  // int Duration = 100;
  Serial.println("Test Start");
  Stop();
  for ( j = 0; ++j <= Duration; ) {
    digitalWrite(IN1, 1);
    Serial.println("IN1");
  }
  Stop();
  for ( j = 0; ++j <= Duration; ) {
    digitalWrite(IN2, 1);
    Serial.println("IN2");
  }
  Stop();
  for ( j = 0; ++j <= Duration; ) {
    digitalWrite(IN3, 1);
    Serial.println("IN3");
  }
  Stop();
  for ( j = 0; ++j <= Duration; ) {
    digitalWrite(IN4, 1);
    Serial.println("IN4");
  }
  Stop();
  for ( j = 0; ++j <= Duration; ) {
    digitalWrite(IN5, 1);
    Serial.println("IN5");
  }
  Stop();
  for ( j = 0; ++j <= Duration; ) {
    digitalWrite(IN6, 1);
    Serial.println("IN6");
  }
  Stop();
  Serial.println("Test End");
}


// Test motora uz pomoć pauze
void DTest(int Duration) {
  // int Duration = 100;
  // Ostani u pauzi 100 milisekundi
  Serial.println("Test Start");
  Stop();
  digitalWrite(IN1, 1); Serial.println("IN1"); delay(Duration);
  Stop();
  digitalWrite(IN2, 1); Serial.println("IN2"); delay(Duration);
  Stop();
  digitalWrite(IN3, 1); Serial.println("IN3"); delay(Duration);
  Stop();
  digitalWrite(IN4, 1); Serial.println("IN4"); delay(Duration);
  Stop();
  digitalWrite(IN5, 1); Serial.println("IN5"); delay(Duration);
  Stop();
  digitalWrite(IN6, 1); Serial.println("IN6"); delay(Duration);
  Stop();
  Serial.println("Test End");
}

// Stop function
void Stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  digitalWrite(IN5, LOW);
  digitalWrite(IN6, LOW);
}

// Forward function
void Forward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, 1);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, 1);
}

// Back function
void Back() {
  digitalWrite(IN1, 1);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, 1);
  digitalWrite(IN4, LOW);
}

// Turn  left
void Turn_Left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, 1);
  digitalWrite(IN3, 1);
  digitalWrite(IN4, LOW);
}

// Turn  right
void Turn_Right() {
  digitalWrite(IN1, 1);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, 1);
}

void Motor_Up() {
  digitalWrite(IN5, LOW);
  digitalWrite(IN6, 1);
}

void Motor_Down() {
  digitalWrite(IN5, 1);
  digitalWrite(IN6, LOW);
}

void Mala_udaljenost()
{

  for ( j = 0; ++j <= 100; ) //
  {
    Turn_Left();
    Serial.println("Left");
  }
  for ( j = 0; ++j <= 100; ) //
  {
    Turn_Right();
    Serial.println("Right");
  }

}

void setup(void)
{
  Motor_Init();
  Stop();

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

  pinMode(PinPiezo, OUTPUT);
}

void senzor()
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
  TFT.print("--: ");

  distance_old = distance;  //Novo očitanu vrijednost postavi kao staru

  TFT.setTextColor(ILI9341_GREEN); // postavljamo boju teksta u zelenu
  TFT.setCursor(0, 0);
  TFT.print("Distance(mm): "); //Ispiši novu vrijednost
  TFT.print(distance);
  TFT.setCursor(0, 10);
  TFT.print("--: ");

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
}

void brzi_senzor()
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
  }
  distance_old = distance;  //Novo očitanu vrijednost postavi kao staru

  for (int o = 0; o < brojUzoraka; o = o + 1) { // Premještamo niz kako bi napravili mjesta za novu vrijednost
    distValues[o] = distValues[o + 1];
  }
  distValues[brojUzoraka - 1] = distance;     // Spremi vrijednost senzora na zadnje mjesto niza
  srednjaVreijdnost = 0;
  for (int o = 0; o < brojUzoraka; o = o + 1) { // Pripremamo varijablu za računanje aritmetičke sredine
    srednjaVreijdnost = srednjaVreijdnost + distValues[o];
  }
  srednjaVreijdnost = srednjaVreijdnost / brojUzoraka; // računamo artitmetičku sredinu
}

void izmjeri_udaljenost() // Izmjeri udaljenost oko sebe
{
  for (int o = 0; o < 16; o = o + 1) { // Premještamo niz kako bi napravili mjesta za novu vrijednost
    Turn_Left();
    delay(vrijednost); // Treba odrediti "vrijednost" kojom će tenk napraviti puni krug u 16 prolaza petlje
    Stop();
    brzi_senzor();
    circValues[o] = distance;
    // Ispisujemo vrijednosti udaljenosti na ekran
    TFT.print(" dist(");
    TFT.print(o);
    TFT.print(")= ");
    TFT.println(circValues[o]);

  }
}

void mjeri_najdalje() // Gdje je najdalje mjerenje
{
  najdalje = 0;
  element = -1;
  for (int o = 0; o < 16; o = o + 1) { // Premještamo niz kako bi napravili mjesta za novu vrijednost
    if ( circValues[o] > najdalje )
    {
      najdalje = circValues[o];
      element = o;
    }
  }
  // Ispisujemo vrijednosti udaljenosti na ekran
  TFT.print("Najdalje je ");
  TFT.print(element);
  TFT.print(". element niza: ");
  TFT.println(najdalje);
}

void loop(void)
{
  Stop();
  //Turn_Left();
  TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
  TFT.setTextColor(ILI9341_GREEN); // postavljamo boju teksta u zelenu
  TFT.setCursor(0, 0);

  izmjeri_udaljenost();
  mjeri_najdalje(); // Gdje je najdalje mjerenje
  // Okreni se prema najdaljoj udaljenosti
  TFT.print("Okrecem se za ");
  TFT.print(15 - element);
  TFT.print(".");
  TFT.println(" ");
  for (int o = 0; o < ( 15 - element ); o = o + 1) {
    Turn_Right();
    delay(vrijednost); // Treba odrediti "vrijednost" kojom će tenk napraviti puni krug u 16 prolaza petlje
    Stop();
  }
  TFT.print("Vozim ravno ");
  TFT.print(najdalje * 10);
  TFT.print(" milisekundi");
  TFT.println(" ");
  Forward();
  delay( najdalje * 10 ); // Kreći se naprijed prema nekom mjerenju
  Stop(); // Zaustavi sve motore
  delay(9000); // Odmori zbog testiranja
  // Možemo krenuti ispočetka mjeriti udaljenosti

  // DTest(1000); // Koristili smo za testiranje spojeva
  // Mala_udaljenost();
}
