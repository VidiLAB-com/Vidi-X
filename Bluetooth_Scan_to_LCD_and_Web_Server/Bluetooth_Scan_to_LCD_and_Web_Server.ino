//--------------------------------------------------------------------
//----------------------- VIDI 287 -----------------------------------
//------- Pošalji pronađene Bluetooth uređaje na Web server ----------
//--- napisan za potrebe članka u časopisu VIDI 287 - veljača 2020. --
//--------------------------------------------------------------------
//
// Ovaj kod radi s VIDI project X pločicom o kojoj možete saznati na linku
// https://www.vidi.hr/Racunala/Novosti/VIDI-e-novation-Predstavljeno-VIDI-Project-X-mikroracunalo
//  
//-------------------------------------------------------------------
// napisao: Hrvoje Šomođi, Vidi - 17.1.2020.
//-------------------------------------------------------------------
// Nedostaju li vam niže spomenuti libraryji
// odaberite Manage Libraries sa izboranika Alti
// zatim u tražilicu upišite ILI9341
// i instalirajte ponuđenu Adafruit ILI9341 librari
// te ostale librarije koji su ponuđeni uz ovu!
// libraryji za Bluetooth te Wi-Fi trebali su
// biti instalirani kada ste instalirali podršku za ESP32

// dodajemo library za Bluetooth 
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; // Vrijeme skeniranja u sekundama
BLEScan* pBLEScan;

// dodajemo library za ekran 
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// Za VIDI X ekran ovako trebaju biti deklarirani pinovi.
#define TFT_DC 21
#define TFT_CS 5

// dodajemo library za Wi-Fi
#include <WiFi.h>

const char* ssid     = "VIDI_X"; // Umjesto VIDI_X trebate upisati ime vaše Wi-Fi mreže na koju se spajate
const char* password = "123546"; // Umjesto 123456 trebate upisati password za vašu Wi-Fi mrežu na koju se spajate

//Ovo će stvoriti Web server koji će osluškivati na portu 80
WiFiServer server(80);

int value = 0;
String stringThree = "";

// Ovo će kreirati tft objek za korištenje ekrana
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// poziv za skeniranje Bluetooth uređaja
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      // ispiši rezultat na serial monitor jer se on konstatno osvježava
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      // postavi boju tekta na crvenu za lakše raspoznavanje
      tft.setTextColor(ILI9341_RED);
      // ispiši
      tft.print("Device: ");
      // postavi boju teksta na zelenu
      tft.setTextColor(ILI9341_GREEN);
      // ispiši pronađeni Bluetooth uređaj
      tft.println(advertisedDevice.toString().c_str());
      // dodaj pronađeni Bluetooth uređaj u varijablu za kasnije korištenje
      // na kraj stringa dodaj HTML prelazak u novi red
      // ovdje je moguće dodati i HTML kod za promjenu boje, fonta 
      // i ostale HTML kompatibilne kodove 
      stringThree = stringThree + advertisedDevice.toString().c_str() + "<br>";
    }
};

void setup() {
  // postavi serial monitor
  Serial.begin(115200);
  Serial.println("Scanning...");

  // iniciajlizira ekran
  tft.begin();
  tft.setRotation(3);
  tft.setTextSize(1);
  
  //pozovi funkicu za inicijalizaciju Web servera
  serv_setup();

  //inicijaliziraj Bletooth skeniranje
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //stvara novo Bletooth skeniranje
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //aktivno skeniranje troši više struje no efikasnije je
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // ovo mora biti manje ili jednako setInterval vrijednosti
}

void serv_setup()
{
    // Prvo je potrebno spojiti se na postojeću Wi-Fi mrežu
    // Ispisujemo o kojoj Wi-Fi mreži je rijeć
    Serial.print("Connecting to ");
    Serial.println(ssid); // ispisujemo na serial moniotr
    
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(ILI9341_YELLOW);
    tft.print("Connecting to ");
    tft.println(ssid); // ispisujemo na ekran

    // spajamo se sa gore navedenim passwordom na navedneu Wi-Fi mrežu
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) { // dok ćekamo na ostavrenje veze
        delay(500);
        Serial.print("."); // ispisuje točku
        tft.print(".");    // na ekran
    }
    // kada smo se pojili ispiši IP adresu servera
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    tft.println("WiFi connected.");
    tft.println("IP address: ");
    tft.print(WiFi.localIP()); // IP adresu ispisujemo na ekran
    tft.println(" ");
    // startamo server
    server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);

  WiFiClient client = server.available();   // osluškuj spaja li se netko na web server

  if (client) {                             // ako se client spojio na web server,
    Serial.println("New Client.");          // ispiši poruku o tome na serial port
    String currentLine = "";                // stvori String varijablu koja će sadržavati dolazeće podatke od klijenta
    
    while (client.connected()) {            // while petlja dok je klijent spojent
      
      if (client.available()) {             // ukoliko klijent šallje podatke
        char c = client.read();             // pročitaj ih
        Serial.write(c);                    // ispiši pročitano na serial monitor
        
        if (c == '\n') {                    // ako je pročitan znak za novi red
          // ako je trenutni redak prazan, dobit ćete dva nova retka u nizu
          // to je kraj klijentskog HTTP zahtjeva, pa pošaljite odgovor
          if (currentLine.length() == 0) {
            // HTTP headers uvijek započnite s kodom odgovora (e.g. HTTP/1.1 200 OK)
            // i uznakom za tip sadržaja, content-type, pa klijent zna što očekivati, a zatim treba poslati prazan redak
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // sadržaj HTTP odgovora prati zaglavlje
            client.print("Devices found: ");
            client.print(foundDevices.getCount());
            client.print("<br>");

            // nakon zaglavlja ispisati ćemo string varijablu u koju
            // smo spremili native Bletooth uređaja
            client.print(stringThree);

            // sadržaj HTTP odgovora završava s parznim redom:
            client.println();
            // break naredba izlazi iz while petlje:
            break;
          } else {    // ako ste dobili novi redak, očistite varijablu currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // ako je sever dobio još nešto osim zanaka za kraj,
          currentLine += c;      // dodaj to na kraj varijable currentLine
        }
      }
    }
    // zatvara konekciju i ispiši poruku o tome:
    client.stop();
    Serial.println("Client Disconnected.");
  }

  // ispiši broj pronađenih Bluetooth uređaja i na serial monitor
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  
  // ispiši broj pronađenih Bluetooth uređaja na tft ekran
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_YELLOW);
  tft.print("Devices found: ");
  tft.print(foundDevices.getCount());
  tft.println(" ");

  pBLEScan->clearResults(); // očisti rezultate scaniranja iz fromBLEScan buffera  kako bi uštedjeli na momoriji
  stringThree = "";         // očisti string varijablu kako bi ju pripremili za novi unos
  delay(2000); // pričekaj dvije sekunde prije ponovnog prolaza kroz glavnu petlju programa
}
