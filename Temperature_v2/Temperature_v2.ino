//--------------------------------------------------------------------
//------------------------ VIDI 297 ----------------------------------
//-------------------- kod za Termostat centralnog grijanja ----------
//-- napisan za potrebe članka u časopisu VIDI 297 - prosinac 2020. --
//--------------------------------------------------------------------
//
// Kod radi s VIDI project X pločicom o kojoj možete saznati na linku
// https://vidilab.com/vidi-project-x
//
//
//-------------------------------------------------------------------
// napisao: Hrvoje Šomođi, Vidi - 30.10.2020. - verzija 0.01
//-------------------------------------------------------------------
//
// Nedostaju li vam niže spomenuti libraryji
// odaberite Manage Libraries sa izboranika Alati
// zatim u tražilicu upišite ILI9341
// i instalirajte ponuđenu Adafruit ILI9341 librari
// te ostale librarije koji su ponuđeni uz ovu!
//
// analogno 747 = 22.0°C
// analogno 673 = 19.0°C
// analogno 615 = 10.5°C
// analogno 505 = 06.5°C
//
// temperature = ((float)analogRead(A0) * 3.3 / 4096.0) - 0.33;
// temperature = temperature / 0.01;
//
// OSTAJE ZA NAPRAVITI U BUDUĆIM VERZIJAMA
// - Dohvaćanje trenutnog vremena sa NTP Servera
// - Zapisivanje statusa temperature i releya u realnom vremenu na SD karticu
// - Zapisivanje statusa temperature i releya u realnom vremenu u MySQL bazu
// - Postavljamo MENU za unošenje kalibracijskih vrijednosti
// - Detektiramo prisutnost ukućana kako bi termostat mogao procijeniti preferiranu vrijednost temperature prema željama pojedinog ukućana
// - Spremamo u učitavamo konfiguraciju tj. setup vrijednosti sa SD kartice
// - Mogućnost programiranja putem BT konekcije
// - Mogućnost programiranja putem Wi-Fi konekcije
// - Kontrola uz Touch screen ...

#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include <SPI.h>

// ILI9341 TFT LCD deklaracija spajanja zaslona
#define TFT_CS   5
#define TFT_DC  21
#define TFT_RST  4

#define UP_DOWN  35
#define Gumb_Arm 32

bool arm = false;
const int RELAY = 13;
const int stat = 2;

// Stvori objekt zaslona koji zovemo TFT
Adafruit_ILI9341 TFT = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Analogni pin 14 spojen je na mikrofon sa mikrodfonskim predpojačalom
const int micInPin = 14;      // Input pin mikrofona za implementaciju reakcije na zvuk
const int analogInPin = 26;   // Analogni input pin Senzora Temperature

// varijable za dimenzije zaslona
int myWidth;
int myHeight;
int temperatura = 715; // analogno 747 = 22.0°C
float ciljTemp;
const int brojUzoraka = 10;  // broj uzoraka tempearture spremljene u niz, Mora biti contanta
int tempValues[brojUzoraka] = { 600, 600, 600, 600, 600, 600, 600, 600, 600, 600}; // niz
float srednjaVreijdnost;
int j = 0;             // inicijaliziramo varijeble koje će nam trebati
int n = 0;             // varijabla za glavnu petlju programa, umjesto delay 10000
int val = 0;
int valMin = 4096;     // ove varijeble postavljemo na besmisleno velike vrijednosti
int valMax = -4096;    // ..koje nam softver neće javiti
int GumbUP;            // Za vrijednost gumba gore dolje

//float tempC;
float tempC1;
int analogRef;

void pozdravniEkran() {
  TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
  //TFT.setCursor(0, 0);
  TFT.setTextSize(2);
  TFT.setTextColor(ILI9341_CYAN);
  TFT.print("VIDI X mikroracunalo kao  ");
  TFT.print("TERMOSTAT CENTRALNOG                GRIJANJA         ");
  TFT.setTextSize(1);
  TFT.setTextColor(ILI9341_YELLOW);
  TFT.print("Prije koristenja temperaturnog senzora ");
  TFT.setTextColor(ILI9341_GREEN);
  TFT.println("MCP9700AT ");
  TFT.setTextColor(ILI9341_YELLOW);
  TFT.println("potrebno ga je kalibrirati. ");
  delay(1000);
  TFT.print("Toj svrsi posluziti ce ");
  TFT.setTextColor(ILI9341_GREEN);
  TFT.println("DeltaMin i DelataMax ");
  TFT.setTextColor(ILI9341_YELLOW);
  TFT.println("varijable koje oznacavaju analogne min. i max. vrijednosti zabiljezenih temperatura. ");
  delay(1000);
  TFT.print(" Ciljanu temperaturu namijestate uz pomoc ");
  TFT.setTextColor(ILI9341_GREEN);
  TFT.print("UP i DOWN tipki ");
  TFT.setTextColor(ILI9341_YELLOW);
  TFT.print(", dok tipkom ");
  TFT.setTextColor(ILI9341_GREEN);
  TFT.print("A ");
  TFT.setTextColor(ILI9341_YELLOW);
  TFT.print(" mijenjate ");
  TFT.setTextColor(ILI9341_GREEN);
  TFT.print("WEAPONIZED ");
  TFT.setTextColor(ILI9341_YELLOW);
  TFT.print("vrijednost koja odreduje ");
  TFT.print("zelite li paliti i gasiti termostat (1) ili samo zelite simulirati ");
  TFT.println("koristenje termostata plavom ledicom (0). ");
  delay(2000);
  TFT.setTextColor(ILI9341_RED);
  TFT.print(" Ne zaboravi postaviti mikro prekidac pina 13 - MENU - na poziciju GAME USE - kako bi se novi kod moago uploadati, odnosno ");
  TFT.println("postaviti ga na poziciju USE EXP. za korištenje RELAYa. ");
  delay(1000);
  TFT.print(" Isto tako ne zaboravite postaviti mikroprekidac TOUCH_IRQ ga na poziciju USE EXP. za upload novoga koda. ");
  delay(2000);
  TFT.setTextColor(ILI9341_GREEN);
  TFT.print(" Za start pritisni tipku UP ili DOWN! ");
  TFT.println(" Senzoru treba minuta dvije da postigne radnu temperaturu. ");
}

float temperature(int analog) {
  double temp = analog * 3.30 / 4095.0; // Analognu vrijednost pretvaramo u otpor
  temp = temp - 0.40;                  // Oduzimamo 500mA tj. -40°C
  temp = temp / 0.01;                  // Pretvaramo vrijednosti u °C
  return temp;
}

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

void setup() {
  pinMode(analogInPin, INPUT);     // Postavi pin senzora kao INPUT
  pinMode(RELAY, OUTPUT);          // pin Relay-a deklariramo kao Output pin
  digitalWrite(RELAY, LOW);        // Postavljamo ga na LOW jer pin 0 pri restartu ima slučajnu analognu vrijednost
  pinMode(stat, OUTPUT);           // pin Status LEDa deklariramo kao Output pin
  pinMode(UP_DOWN, INPUT);         // Postavi gore dolje gumb kao INPUT
  //pinMode(Gumb_Arm, INPUT);      // Postavi gumb A kao INPUT
  pinMode(Gumb_Arm, INPUT_PULLUP); // A gumb mora biti definiran kao INPUT_PULLUP

  TFT.begin();                   // inicijalizacuija zaslona
  TFT.setRotation(3);            // postavi orijentaciju
  myWidth  = TFT.width() ;       // ekran je širok
  myHeight = TFT.height();       // ekran je visok

  pozdravniEkran();              // pozovi ispisivanje pozdravnog teksta

  while (!analogRead(UP_DOWN)) { // izvodi se petlja dok ne pritisnemo gumb
    n++;
    TFT.setCursor(0, 200);
    TFT.fillRect(0, 200, 320, 240, ILI9341_BLACK); // Brišemo stari tekst sa ekrana
    TFT.print(n);                              // Samo za debuging
    if (n > 5000) { // Nakon određenog vremena
      break;        // izađi iz while petlje
    }
  }

  TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
  n = 1000000 - 1; // Postavljamo vrijednost n varijable kako bi se glavna petklja odmah izvršila
}

void loop() {
  n++;                // Ako je varijabla n dosegla vrijednost 1000000 pokreni glavni dio programa
  if (n == 1000000) { // Smanjivanjem ovog broja, glavni dio programa se češće izvodi
    n = 0;

    j++;                   // Varijabla j nam treba za kretanje po X-osi ekrana i iscrtavanje pixela
    if (j > myWidth + 1) { // Kada j izađe iz gabarita ekrana
      j = 0;               // resetriraj vrijednost na početak ekrana
    }
    TFT.fillRect(0, 0, 320, 30, ILI9341_BLACK); //Brišemo stari tekst sa ekrana

    val = analogRead(analogInPin);       // Spremi vrijednost senzora u varijablu
    for (int o = 0; o < brojUzoraka; o = o + 1) { // Premještamo niz kako bi napravili mjesta za novu vrijednost
      tempValues[o] = tempValues[o + 1];
    }
    tempValues[brojUzoraka - 1] = val;     // Spremi vrijednost senzora na zadnje mjesto niza
    srednjaVreijdnost = 0;
    for (int o = 0; o < brojUzoraka; o = o + 1) { // Pripremamo varijablu za računanje aritmetičke sredine
      srednjaVreijdnost = srednjaVreijdnost + tempValues[o];
      //TFT.print(" srednjaVreijdnost=");
      //TFT.print(srednjaVreijdnost);
      //TFT.print(" tempValues[o]=");
      //TFT.print(tempValues[o]);
      //    TFT.print(" [o]=");
      //TFT.print(o);
    }
    srednjaVreijdnost = srednjaVreijdnost / brojUzoraka; // računamo artitmetičku sredinu
    ciljTemp = temperature(temperatura);
    tempC1 = temperature(srednjaVreijdnost); // Pozivamo funkciju za pretvorbu analognih očitanja u stupnjeve Cezijeve
    //tempC1 = temperature(val); //ovo nam više ne treba jer smo računali srednju vrijednost

    if (valMin > val) { // Ukoliko je dosegnut novi minimum
      valMin = val;     // postavi DeltaMin vrijednost
    }
    if (valMax < val) {  // Ukoliko je dosegnut novi maksimum
      valMax = val;      // postavi DeltaMax vrijednost
    }

    if ( arm ) {
      if (srednjaVreijdnost > temperatura ) {
        digitalWrite(RELAY, LOW);
        digitalWrite(stat, LOW);
      }
      if (srednjaVreijdnost <= temperatura) {
        digitalWrite(RELAY, HIGH);
        digitalWrite(stat, HIGH);
      }
    } else {
      digitalWrite(RELAY, LOW);
      // ovaj dio koda se samo za simulaciju
      // kako bi mogli kalibrirati senzor prije korištenja
      if (srednjaVreijdnost > temperatura ) {
        digitalWrite(stat, LOW);
      }
      if (srednjaVreijdnost <= temperatura) {
        digitalWrite(stat, HIGH);
      }
    }

    TFT.setTextColor(ILI9341_GREEN);
    dijeljenje(j); // postavljamo boju teksta
    TFT.setCursor(0, 0);
    TFT.print("DeltaMin = ");
    TFT.print(valMin); // ispisujemo na ekran
    TFT.setTextColor(ILI9341_RED);
    dijeljenje(j + 1); // postavljamo boju +1
    TFT.print(" DeltaMax = ");
    TFT.print(valMax); // ispisujemo na ekran
    TFT.setTextColor(ILI9341_BLUE);
    dijeljenje(j + 2); // postavljamo boju +2
    TFT.print(" Temp = ");
    TFT.print(val); // ispisujemo na ekran
    TFT.print("      ");
    TFT.setCursor(0, 10);
    TFT.print("CiljanaTemp = ");
    TFT.print(ciljTemp);
    TFT.print(" TempC1 = ");
    TFT.print(tempC1);
    TFT.setCursor(0, 20);
    TFT.print("varijabla = ");
    TFT.print(temperatura);
    TFT.setCursor(180, 20);
    TFT.print("WEAPONIZED = ");
    TFT.print(arm); // ispisujemo na ekran
    // Crtanje grafikona na ekranu
    TFT.drawFastVLine( j, 0, myHeight - 40, ILI9341_BLACK); // brišemo staru krivulju kako bi mogli nacrtati novu na njezinom mjestu
    TFT.drawPixel(j, map(val, 0, 4095, myHeight - 1, 1), ILI9341_YELLOW); // Crtamo krivulju temperature na ekranu
    TFT.drawPixel(j, map(val, 500, 1000, myHeight - 41, 1), ILI9341_RED); // Crtamo izraženiju krivulju temperature
    TFT.drawPixel(j, map(srednjaVreijdnost, 500, 1000, myHeight - 91, 1), ILI9341_GREEN); //Crtamo srednju vrijednost
  } // kraj glavne petlje // if (n == 100000000)

  GumbUP = analogRead(UP_DOWN);
  if ( !digitalRead(Gumb_Arm) ) {
    arm = !arm;
    n = 1000000 - 1;
    delay(500); //Kada smo pritisnuli gumb trebamo odgodu
  }

  if (GumbUP > 0 ) {
    if (GumbUP < 2048 ) {
      temperatura = temperatura - 1;
      if (temperatura < 700) {
        temperatura = 700;
      }
      n = 1000000 - 1; // Postavljamo varijablu kako bi se pokrenuo glavni dio koda
    } else {
      temperatura = temperatura + 1;
      if (temperatura > 800) {
        temperatura = 800;
      }
      n = 1000000 - 1; // Postavljamo varijablu kako bi se pokrenuo glavni dio koda
    }
    delay(300); //Kada smo pritisnuli gumb trebamo odgodu
  }
}
