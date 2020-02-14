 //////////////////////////////////////VIDI////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////6.2.2020./////////////////////////////////////
////////IZRADIO://////IVOR///S.///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////Project X plocica na ekranu prikazuje izbornik////////////////////////////////
////Na ekranu se nalazi glavni izbornik s dvjema opcijama odabira 1 i 2///////////
////Izbor1 i Izbor2 nude mogucnost povrata na glavni izbornik i///////////////////
////Upravljanja LEDicom. Kroz izbornik se krece uz pomoc tipki////////////////////
////ugradjenih na Project X plocici, tipke A i B./////////////////////////////////
////Tipkom A odabiremo izbor1, a tipkom B odabiremo izbor2.///////////////////////
////Unutar izbora1 i izbora2 tipkom A vraca se u pocetni izbornik, a /////////////
////Tipkom B ukljucuje se LEDica stat na plocici(plava)///////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include <TFT_eSPI.h>
#include <SPI.h>
#define Tipka_A 32                               //Tipka A na plocici
#define Tipka_B 33                               //Tipka B na plocici
#define LED 2                                    //LEDica stat na plocici(plava)
int izbor1, izbor2, pocetni;                     //Pomocne varijable za izbornik
TFT_eSPI zaslon = TFT_eSPI();                    //Ime ekrana - zaslon

void setup() {
  pinMode(Tipka_A, INPUT_PULLUP);                //Tipka A definirana kao ulaz
  pinMode(Tipka_B, INPUT_PULLUP);                //Tipka B definirana kao ulaz
  pinMode(LED, OUTPUT);                          //LEDica definirana kao izlaz
  pocetni = 1;                                   //Pomocne varijable za pozicije u izborniku
  izbor1 = 0;                                    //Pri pocetku program postavlja se u pocetni izbornik
  izbor2 = 0;                                    //Ostale varijable trebaju biti 0
  zaslon.init();                                 //inicijalizacija zaslona
  zaslon.setRotation(3);                         //orijentacija zaslona (vodoravno, ispravno kada se drzi plocica u rukama)
  pocetniz();                                    //Poziv pomocne funkcije za odabir pocetnog izbornika
}

void loop() {
  if(pocetni && !izbor1 && !izbor2){             //Ako je pokrenut pocetni izbornik
    if(!digitalRead(Tipka_A)){                   //Na pritisak Tipke A prelazi u Izbor1
      pocetni = 0;                               //varijabla za pocetni = 0
      izbor1 = 1;                                //varijabla za izbor1 = 1
      izbor2 = 0;                                //varijabla za izbor2 = 0
      izborx();                                  //poziv pomocne funkcije za izbor1
      delay(100);                                //pauza od 100mS kako bi korisnik stigao otpustiti pritisak
    }
    if(!digitalRead(Tipka_B)){                   //Na pritisak Tipke B prelazi u Izbor2
      pocetni = 0;                               //varijabla za pocetni = 0
      izbor1 = 0;                                //varijabla za izbor1 = 0
      izbor2 = 1;                                //varijabla za izbor2 = 1
      izbory();                                  //poziv pomocne funkcije za izbor2
      delay(100);                                //pauza od 100mS kako bi korisnik stigao otpustiti pritisak
    }
  }
  if(!pocetni && izbor1 && !izbor2){             //Ako je pokrenut izbor1
    if(!digitalRead(Tipka_A)){                   //Na pritisak tipke A vraca se u pocetni izbornik
      pocetni = 1;                               //varijabla za pocetni = 1
      izbor1 = 0;                                //varijabla za izbor1 = 0
      izbor2 = 0;                                //varijabla za izbor2 = 0
      pocetniz();                                //poziv pomocne funkcije za pocetni izbornik
      delay(100);                                //pauza od 100mS kako bi korisnik stigao otpustiti pritisak
    }
    if(!digitalRead(Tipka_B)){                   //Na pritisak tipke B ukljucuje se LEDica
      digitalWrite(LED, HIGH);                   
    } else {                                     
      digitalWrite(LED, LOW);                    
    }
  }
  if(!pocetni && !izbor1 && izbor2){             //Ako je pokrenut izbor2
    if(!digitalRead(Tipka_A)){                   //Na pritisak tipke A vraca se u pocetni izbornik
      pocetni = 1;                               //varijabla za pocetni = 1
      izbor1 = 0;                                //varijabla za izbor1 = 0
      izbor2 = 0;                                //varijabla za izbor2 = 0
      pocetniz();                                //poziv pomocne funkcije za pocetni izbornik
      delay(100);                                //pauza od 100mS kako bi korisnik stigao otpustiti pritisak
    }
    if(!digitalRead(Tipka_B)){                   //Na pritisak tipke B ukljucuje se LEDica
      digitalWrite(LED, HIGH);                   
    } else {                                     
      digitalWrite(LED, LOW);                    
    }
  }
}

void izborx(){                                   //Pomocna fnkcija za izbor1
  
  zaslon.fillScreen(TFT_BLACK);                  //Pozadina zaslona - crna
  zaslon.setCursor(100, 20);                     //Pozicioniraj kursor na ekranu
  zaslon.setTextColor(TFT_GREEN);                //Postavi boju teksta u Zelenu
  zaslon.setTextFont(4);                         //Font velicine 4
  zaslon.println("Izbor 1");                     //Ispis izbornika
  zaslon.setCursor(60, 90);                      //Pozicioniraj kursor na ekranu
  zaslon.setTextColor(TFT_BLUE);                 //Postavi boju teksta u plavu
  zaslon.setTextFont(4);                         //Font velicine 4
  zaslon.println("Pocetni..........A");          //Ispis prvog odabira
  zaslon.setCursor(60, 140);                     //Pozicioniraj kursor na ekranu
  zaslon.println("LEDica...........B");          //Ispis drugog odabira
}
void izbory(){                                   //Pomocna funkcija za izbor2
  
  zaslon.fillScreen(TFT_BLACK);                  //Pozadina zaslona - crna
  zaslon.setCursor(100, 20);                     //Pozicioniraj kursor na ekranu
  zaslon.setTextColor(TFT_GREEN);                //Postavi boju teksta u Zelenu
  zaslon.setTextFont(4);                         //Font velicine 4
  zaslon.println("Izbor 2");                     //Ispis izbornika
  zaslon.setCursor(60, 90);                      //Pozicioniraj kursor na ekranu
  zaslon.setTextColor(TFT_BLUE);                 //Postavi boju teksta u plavu
  zaslon.setTextFont(4);                         //Font velicine 4
  zaslon.println("Pocetni..........A");          //Ispis prvog odabira
  zaslon.setCursor(60, 140);                     //Pozicioniraj kursor na ekranu
  zaslon.println("LEDica...........B");          //Ispis drugog odabira
}
void pocetniz(){                                 //Pomocna funkcija za pocetni izbornik

  zaslon.fillScreen(TFT_BLACK);                  //Pozadina zaslona - crna
  zaslon.setCursor(60, 20);                      //Pozicioniraj kursor na ekranu
  zaslon.setTextColor(TFT_GREEN);                //Postavi boju teksta u Zelenu
  zaslon.setTextFont(4);                         //Font velicine 4
  zaslon.println("Pocetni zaslon");              //Ispis pocetnog zaslona
  zaslon.setCursor(60, 90);                      //Pozicioniraj kursor na ekranu
  zaslon.setTextColor(TFT_BLUE);                 //Postavi boju teksta u plavu
  zaslon.setTextFont(4);                         //Font velicine 4
  zaslon.println("Izbor 1..........A");          //Ispis prvog odabira
  zaslon.setCursor(60, 140);                     //Pozicioniraj kursor na ekranu
  zaslon.println("Izbor 2..........B");          //Ispis drugog odabira
}
