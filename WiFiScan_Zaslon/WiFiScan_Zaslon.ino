//////////////////////////////////////VIDI////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////6.2.2020./////////////////////////////////////
////////IZRADIO://////IVOR///S.///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////Pritiskom na tipku A koja se nalazi na plocici, pokrece se skeniranje/////////
////WiFi mreza te se ispisuje na ugradjenom ekranu////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include <TFT_eSPI.h>
#include <SPI.h>
#include "WiFi.h"
#define Tipka_A 32                               //Tipka A na plocici spojena na pin 32 
int n, i;                                        //i - brojac, n - broj mreza
TFT_eSPI zaslon = TFT_eSPI();                    //Ime ekrana - zaslon

void setup()
{
    zaslon.init();                               //inicijalizacija zaslona
    zaslon.setRotation(3);                       //orijentacija zaslona (vodoravno, ispravno kada se drzi plocica u rukama)
    zaslon.fillScreen(TFT_BLACK);                //Pozadina zaslona - crna
    pinMode(Tipka_A, INPUT_PULLUP);              //Tipka A s plocice
    WiFi.mode(WIFI_STA);                         //Postavi WiFi u station mode
    WiFi.disconnect(); 
    delay(100);                                  //Pricekaj 100mS
    zaslon.setCursor(35, 35);                    //Pozicioniraj kursor na ekranu
    zaslon.setTextColor(TFT_GREEN);              //Postavi boju teksta u Zelenu
    zaslon.setTextFont(1);                       //Font velicine 2
    zaslon.println("Skeniranje moze poceti klikom na tipku A!");   //Obavijest kad je spreman za skeniranje
}

void loop()
{
  if(!digitalRead(Tipka_A)){                     //Na pritisak tipke A
    skeniraj();                                  //Pozovi funkciju skeniraj
  }
  delay(10);                                     //Ispituj tipku A svakih 10mS
}

//Funkcija "skeniraj" tipa void, izdvojena je iz glavne petlje, ali moze biti izvedeno i bez dodatne funkcije
//Funkcija se poziva u glavnoj petlji pri pritisku na Tipku A i izvrsava se
//Void tip funkcije ne vraća nikakav argument, dok bi funkcija tipa int vratila neki cijeli broj
//To na primjer moze biti broj mreza koje su pronadjene (0,1,2,...)

void skeniraj(){                                 //Funkcija skeniraj tipa void
    zaslon.fillScreen(TFT_BLACK);                //Postavi pozadinu u crnu (sve se brise)
    zaslon.setCursor(90, 20);                    //Pozicioniraj kursor na ekranu
    zaslon.setTextColor(TFT_GREEN);              //Postavi boju teksta u Zelenu
    zaslon.setTextFont(1);                       //Font velicine 2
    zaslon.println("Skeniranje zapoceto");       //Obavijest od pocetku skeniranja
    n = WiFi.scanNetworks();                     //Vraća broj pronadjenin mreza
    zaslon.setCursor(90, 40);                    //Pozicioniraj kursor na ekranu
    zaslon.setTextColor(TFT_RED);                //Postavi boju teksta u Zelenu
    zaslon.setTextFont(1);                       //Font velicine 2
    zaslon.println("Skeniranje zavrseno");       //Obavijest o zavrsetku skeniranja
    if(n == 0){                                  //Ako nema mreza
        zaslon.setCursor(85, 60);                //Pozicioniraj kursor na ekranu
        zaslon.setTextColor(TFT_BLUE);           //Postavi boju teksta u Plavu
        zaslon.setTextFont(1);                   //Font velicine 2
        zaslon.println("Mreze nisu pronadjene"); 
    } else{                                      //Ako ima mreza n > 0 
        zaslon.setCursor(90, 80);                //Pozicioniraj kursor na ekranu
        zaslon.setTextColor(TFT_BLUE);           //Postavi boju teksta u Plavu
        zaslon.setTextFont(1);                   //Font velicine 2
        zaslon.print("Pronadjene mreze  ");       
        zaslon.println(n);                       //Ispisi broj pronadjenih mreza
        for (i = 0; i < n; ++i){                 //Prodji redom po mrezama
            zaslon.print(i + 1);                 //Ispisi SSID i RSSI za svaku mrezu
            zaslon.print(": ");
            zaslon.print(WiFi.SSID(i));
            zaslon.print(" (");
            zaslon.print(WiFi.RSSI(i));
            zaslon.print(")");
            zaslon.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    zaslon.println("");                          //Zavrsetak funkcije
}
