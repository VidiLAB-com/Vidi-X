//--------------------------------------------------------------------
//------------------------ VIDI 288 ----------------------------------
//--- Pametni Dom - Kontrola IR uređaja putem interneta --------------
//--- napisan za potrebe članka u časopisu VIDI 288 - ožujak 2020. ---
//--------------------------------------------------------------------
//
// Kod radi s VIDI project X pločicom o kojoj možete saznati na linku
// https://vidilab.com/vidi-project-x
//  
// 
//-------------------------------------------------------------------
// napisao: Hrvoje Šomođi, Vidi - 14.2.2020.
//-------------------------------------------------------------------
//
// Nedostaju li vam niže spomenuti libraryji
// odaberite Manage Libraries sa izboranika Alti
// zatim u tražilicu upišite ILI9341
// i instalirajte ponuđenu Adafruit ILI9341 librari
// te ostale librarije koji su ponuđeni uz ovu!
// zatim u tražilicu upišite IRremoteESP8266
// kako bi instalirali podršku za IR protokol
// 

#include <Arduino.h>
// dodajemo podršku za IR protokol
#include <IRremoteESP8266.h>
#include <IRsend.h>
// dodajemo Wi-Fi library
#include <WiFi.h>
// dodajemo library za ekran 
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// Za VIDI X ekran ovako trebaju biti deklarirani pinovi.
#define TFT_DC 21
#define TFT_CS 5

// Ovo će kreirati tft objek za korištenje ekrana
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

const uint16_t kIrLed = 15;  // GPIO pin VIDI Xa na koji je spojen IR transmit

IRsend irsend(kIrLed);  // Postavi odabrani GPIO za slanje IR signala

// Replace with your network credentials
const char* ssid     = "Ime vaše Wi-Fi mreže";
const char* password = "Password za  vašu Wi-Fi mrežu";

// Postavi web server port na 80, ako ga mijenjamo ovdije treba ga promijenit
// i pri korištenju ngrok naredbe
WiFiServer server(80);

// Variabla za HTTP request
String header;

// Pomoćne variables za pohranu stanja
String linijaVolume = "down";
String linijaPower = "UGASENA";

// IR kodovi koje smo dobili demo skicom „IRrecvDumpV2“ 
uint16_t stisaj1[67] = {4542, 4424,  610, 554,  580, 526,  580, 554,  580, 526,  580, 1630,  612, 552,  580, 526,  580, 552,  582, 524,  580, 552,  580, 526,  580, 552,  580, 1632,  608, 526,  580, 552,  580, 526,  578, 580,  552, 1662,  578, 1664,  576, 556,  550, 1664,  576, 582,  550, 556,  550, 582,  550, 1664,  576, 556,  550, 582,  550, 1664,  576, 556,  550, 1666,  576, 1666,  576, 1638,  576};  // SAMSUNG 8086897
uint16_t stisaj2[67] = {4540, 4426,  610, 552,  580, 526,  580, 552,  580, 526,  580, 1628,  612, 554,  580, 526,  580, 552,  580, 528,  580, 552,  580, 526,  580, 552,  580, 1632,  608, 526,  580, 552,  580, 528,  578, 580,  552, 1662,  578, 1664,  578, 556,  550, 1664,  576, 582,  550, 556,  550, 584,  550, 1666,  576, 556,  550, 582,  550, 1664,  576, 556,  550, 1666,  576, 1664,  576, 1638,  576};  // SAMSUNG 8086897
uint16_t poglasni1[67] = {4540, 4426,  610, 552,  580, 526,  582, 552,  610, 498,  608, 1604,  638, 524,  608, 498,  610, 524,  610, 498,  608, 524,  608, 498,  608, 524,  608, 1604,  636, 498,  606, 526,  606, 500,  604, 1612,  604, 1662,  578, 1662,  578, 558,  576, 1664,  578, 530,  576, 558,  576, 532,  578, 554,  576, 532,  576, 558,  576, 1664,  576, 532,  576, 1664,  576, 1664,  576, 1638,  576};  // SAMSUNG 808E817
uint16_t poglasni2[67] = {4542, 4424,  610, 552,  582, 526,  580, 552,  582, 524,  608, 1604,  638, 524,  610, 498,  608, 524,  610, 498,  608, 524,  608, 498,  610, 524,  608, 1606,  634, 498,  608, 526,  606, 502,  604, 1612,  602, 1662,  578, 1664,  576, 558,  576, 1664,  576, 532,  576, 556,  576, 530,  576, 556,  576, 530,  576, 558,  576, 1664,  576, 530,  576, 1664,  576, 1664,  576, 1638,  576};  // SAMSUNG 808E817
uint16_t rawData1[67] = {4542, 4452,  582, 552,  582, 526,  580, 552,  580, 526,  582, 1658,  582, 552,  582, 526,  580, 552,  580, 526,  580, 552,  582, 524,  582, 552,  582, 1632,  608, 526,  582, 552,  580, 526,  578, 554,  578, 1662,  578, 1662,  578, 1664,  578, 1664,  576, 532,  576, 558,  576, 532,  576, 1664,  576, 558,  576, 532,  576, 556,  576, 532,  576, 1664,  578, 1664,  578, 1638,  576}; 
uint16_t rawData2[67] = {4540, 4426,  610, 526,  606, 524,  582, 550,  582, 526,  582, 1630,  610, 550,  584, 524,  582, 550,  582, 526,  582, 550,  582, 524,  582, 550,  608, 1606,  634, 500,  606, 526,  580, 526,  580, 554,  578, 1662,  578, 1664,  578, 1664,  578, 1664,  576, 530,  576, 556,  576, 530,  576, 1664,  576, 556,  578, 530,  576, 556,  576, 530,  576, 1664,  576, 1664,  576, 1638,  576}; 
uint16_t rawData3[67] = {4538, 4428,  608, 552,  580, 526,  580, 552,  582, 526,  582, 1632,  608, 552,  582, 526,  580, 552,  580, 526,  582, 552,  582, 526,  580, 552,  580, 1634,  606, 526,  580, 552,  580, 526,  578, 554,  578, 1662,  578, 1662,  576, 1664,  576, 1664,  576, 530,  576, 558,  576, 530,  576, 1664,  576, 558,  576, 532,  576, 558,  576, 532,  574, 1666,  576, 1664,  576, 1638,  576}; 
uint16_t rawData4[67] = {4514, 4426,  610, 552,  582, 524,  582, 552,  580, 526,  582, 1632,  608, 552,  582, 526,  582, 550,  582, 526,  580, 552,  580, 526,  582, 552,  582, 1632,  608, 526,  580, 552,  580, 526,  578, 556,  576, 1662,  578, 1664,  576, 1664,  576, 1664,  576, 532,  576, 558,  576, 532,  576, 1664,  576, 556,  576, 532,  576, 558,  576, 532,  576, 1664,  576, 1664,  576, 1638,  576};

void setup() {
  // iniciajlizira ekran
  tft.begin();
  tft.setRotation(3);
  tft.setTextSize(1);
  
  //pozovi funkicu za inicijalizaciju Web servera
  serv_setup();

  //inicijalizacija IR protokola
  irsend.begin();
  #if ESP8266
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  #else  // ESP8266
    Serial.begin(115200, SERIAL_8N1);
  #endif  // ESP8266
}

void loop(){
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // zavisno od stanja pomoćnih varijabli
            // pošalji IR kodove kako slijedi
            if (header.indexOf("GET /26/on") >= 0) {
              tft.println("Linija je poglasnjena");
              linijaVolume = "up";
              irsend.sendRaw(poglasni1, 67, 38);  // Send a raw data capture at 38kHz.
              irsend.sendRaw(poglasni2, 67, 38);  // Send a raw data capture at 38kHz.
            } else if (header.indexOf("GET /26/off") >= 0) {
              tft.println("Linija je stisana");
              linijaVolume = "down";
              irsend.sendRaw(stisaj1, 67, 38);  // Send a raw data capture at 38kHz.
              irsend.sendRaw(stisaj2, 67, 38);  // Send a raw data capture at 38kHz.
            } else if (header.indexOf("GET /27/on") >= 0) {
              tft.println("Linija je upaljena");
              linijaPower = "UPALJENA";
              irsend.sendRaw(rawData1, 67, 38);  // Send a raw data capture at 38kHz.
              irsend.sendRaw(rawData2, 67, 38);  // Send a raw data capture at 38kHz.
              irsend.sendRaw(rawData3, 67, 38);  // Send a raw data capture at 38kHz.
              irsend.sendRaw(rawData4, 67, 38);  // Send a raw data capture at 38kHz.
            } else if (header.indexOf("GET /27/off") >= 0) {
              tft.println("Linija je upaljena");
              linijaPower = "UGASENA";
              irsend.sendRaw(rawData1, 67, 38);  // Send a raw data capture at 38kHz.
              irsend.sendRaw(rawData2, 67, 38);  // Send a raw data capture at 38kHz.
              irsend.sendRaw(rawData3, 67, 38);  // Send a raw data capture at 38kHz.
              irsend.sendRaw(rawData4, 67, 38);  // Send a raw data capture at 38kHz.
            }
            
            // Prikaži HTML stranicu
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS za uređivanje gumbiju 
            // slobodno promijnite ili ih barem obojajte u vaše omiljene boje
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // H1 Heading web stranice
            client.println("<body><h1>VIDI Project X - Web Server za <span style='color: #ff0000'>upravljanje pametnim domom</span></h1>");

            // Pokaži trenutno stanje varijable
            client.println("<p>Linija je: " + linijaPower + "</p>");
            // Prikaži jedan ili drugi gumb
            if (linijaPower=="UGASENA") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            
            // Pokaži zadnje korišteno stanje varijable 
            client.println("<p>State: " + linijaVolume + "</p>");
            // Prikaži oba gumba
            if (linijaVolume=="down") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">POGLASNI</button></a><a href=\"/26/off\"><button class=\"button button2\"><strong>STISAJ</strong></button></a></p>");
            } else {
              client.println("<p><a href=\"/26/on\"><button class=\"button\"><strong>POGLASNI</strong></button></a><a href=\"/26/off\"><button class=\"button button2\">STISAJ</button></a></p>");
            } 
            client.println("</body></html>");
            

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    // Očisti varijablu header
    header = "";
    // Zatvori konekciju
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void serv_setup()
{
    // Prvo je potrebno spojiti se na postojeću Wi-Fi mrežu
    // Ispisujemo o kojoj Wi-Fi mreži je rijeć
    
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
