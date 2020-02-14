//////////////////////////////////////VIDI////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////6.2.2020./////////////////////////////////////
////////IZRADIO://////IVOR///S.///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////Project X plocica pokrece WiFi pristupnu tocku////////////////////////////////
////Na ekranu se ispisuje IP adresa, pomocu mobitela ili racunala/////////////////
////korisnik se spaja na pristupnu tocku i u internet pretrazivacu////////////////
////Chrome, Safari, IE, upisuje navedenu IP adresu i otvara se ///////////////////
////Sucelje pomocu kojeg se moze upravljati s LEDicom, Paliti/gasiti//////////////
////Na zaslonu plocice se takodjer ispisuje kada se korisnik spoji/odspoji////////
////Prilikom vise spajanja/odspajanja na zaslonu se nece osvjezavati//////////////
////Vrijednosti jer nije postavljeno brisanje ekrana(zbog blicanja)///////////////
//////////////////////////////////////////////////////////////////////////////////

#include <TFT_eSPI.h>
#include <SPI.h>
#include <WiFi.h>
#define LED 2

const char *ssid = "Project_X";
const char *password = "nekamojasifra";
WiFiServer server(80);
TFT_eSPI zaslon = TFT_eSPI();                 //Ime ekrana - zaslon

void setup() {
    Serial.begin(115200);
  pinMode(LED, OUTPUT);
  zaslon.init();                              //inicijalizacija zaslona
  zaslon.setRotation(3);                      //orijentacija zaslona (vodoravno, ispravno kada se drzi plocica u rukama)
  zaslon.fillScreen(TFT_BLACK);               //Pozadina zaslona - crna
  zaslon.setCursor(110, 40);                  //Pozicioniraj kursor na ekranu
  zaslon.setTextColor(TFT_GREEN);             //Postavi boju teksta u Zelenu
  zaslon.setTextFont(2);                      //Font velicine 4
  zaslon.println("Postavljanje...");          //Ispis teksta

  WiFi.softAP(ssid, password);                //Lozinka se moze i maknuti
  IPAddress myIP = WiFi.softAPIP();           //Ocitaj postavljenu IP adresu              
  zaslon.setCursor(70, 75);                   //Pozicioniraj kursor na ekranu
  zaslon.setTextColor(TFT_YELLOW);            //Postavi boju teksta u Zutu
  zaslon.print("IP adresa: ");                //Ispis IP adrese
  zaslon.print(myIP);
  server.begin();                             //Pokreni
  zaslon.setCursor(50, 105);                  //Postavi kursor na poziciju
  zaslon.setTextColor(TFT_RED);               //Postavi boju teksta u Crvenu
  zaslon.println("Pristupna tocka pokrenuta"); //Obavijesti o pokrenutom AP
}

void loop() {
  WiFiClient client = server.available();   //Ocekuj klijente

  if (client) {                             //Kada se klijent spoji
    zaslon.setCursor(80, 130);              //Postavi kursor na poziciju
    zaslon.println("Klijent spojen");       //Obavijesti kada se klijent spoji
    String currentLine = "";                //Zadrzi podatke od klijenta u stringu
    while (client.connected()) {            //kada je klijent spojen, uradi....
      if (client.available()) {             //ako ima nesto za procitati od klijenta
        char c = client.read();             //procitaj
        zaslon.setCursor(20, 150);          //Postavi kursor na poziciju
     // zaslon.println(c);                  //ispisi, ali nema potrebe za tim podatcima
        if (c == '\n') {                    // ako je poruka u novom redu...
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");                         //"Response code" uvijek mora na pocetku
            client.println("Content-type:text/html");                  //Stranica je pisana u HTML-u
            client.println();                                          //HTTP zapocinje s praznom linijom

            client.print("<font size=7</font>");                       //Font velicine 7
            client.print("<font face=arial color=red</font>");         //Font-Arial, boja-crvena
            client.print("<center>Upravljaj LEDicom<br></center>");    //Naslov
            client.print("<center><a href=\"/Da\">UKLJUCI</a> LEDicu<br></center>");      //Pritiskom na Ukljuci, ukljucuje se LEDica
            client.print("<center><a href=\"/Ne\">ISKLJUCI</a> LEDicu<br></center>");     //Pritiskom na Iskljuci, iskljucuje se LEDica
            client.println();                                          //HTTP odziv zavrsava s praznom linijom
            break;                                                     //Kraj while petlje
          } else {                                                     //Ako dobijes novu poruku, staru obrisi
            currentLine = "";
          }
        } else if (c != '\r') {                                        //za sve osim poruke vrati karakter
          currentLine += c;                                            //dodaj na kraj currentLine
        }

        if (currentLine.endsWith("GET /Da")) {
          digitalWrite(LED, HIGH);                                     // Ukljuci ledicu na poruku Da
        }
        if (currentLine.endsWith("GET /Ne")) {
          digitalWrite(LED, LOW);                                      // Iskljuci ledicu na poruku Ne
        }
      }
    }
    client.stop();                                                     //Zaustavi komunikaciju
    zaslon.println("Klijent odspojen");                                //Obavijesti o kraju komunikacije
  }
}
