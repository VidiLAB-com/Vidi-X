//////////////////////////////////////VIDI////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////16.1.2020./////////////////////////////////////
////////IZRADIO://////IVOR///S.///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///Pritiskom na Tipku A servo motor se postavlja u poziciju 180°//////////////////
///Pritiskom na Tipku B servo motor se postavlja u poziciju 0°////////////////////
///Ako se servo motor nalazi u poziciji 0° ili 180°, svijetli LEDica status///////
///Program se osvjezava svakih 50mS///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include <TFT_eSPI.h>
#include <ESP32Servo.h> 

Servo motor;                               //Servo naziva motor
const int Tipka_A = 32;                    //Tipka A
const int Tipka_B = 33;                    //Tipka B
const int LED = 2;                         //LEDica za status (plava)
int Stanje_Tipka_A;                        //Varijabla int za stanje tipke A
int Stanje_Tipka_B;                        //Varijabla int za stanje tipke B
TFT_eSPI zaslon = TFT_eSPI();              //Ime ekrana - zaslon
int pozicija = 135;                        //pozicija servo motora

void setup() { 
  motor.attach(12);                        //Servo spojen na GPIO 12
  motor.write(pozicija);                   //Postavi motor na srednju poziciju 135° 
  pinMode(Tipka_A, INPUT_PULLUP);          //Tipka A, ulazni podatak
  pinMode(Tipka_B, INPUT_PULLUP);          //Tipka B, ulazni podatak
  pinMode(LED, OUTPUT);                    //LEDica, izlaz
  zaslon.init();                           //inicijalizacija zaslona
  zaslon.setRotation(3);                   //orijentacija zaslona (vodoravno, ispravno kada se drzi plocica u rukama)
  pinMode(LED, OUTPUT);                    //LEDica, izlaz
  zaslon.fillScreen(TFT_BLACK);            //Pozadina zaslona - crna
}

void loop() {
  Stanje_Tipka_A = digitalRead(Tipka_A);   //Ocitaj stanje tipke A
  Stanje_Tipka_B = digitalRead(Tipka_B);   //Ocitaj stanje tipke B
  
  if(!Stanje_Tipka_A){                     //Tipka_A smanjuje servo u poziciju za 5°
    pozicija+=5;
  }
  if(!Stanje_Tipka_B){                     //Tipka_B povečava servo u poziciju za 5°
    pozicija-=5;
  }

  if(pozicija<0){                          //Ako je pozicija manja od 0, postavi ju na 0 
    pozicija=0;
  }
  if(pozicija>180){                        //Ako je pozicija veća od 180, postavi ju na 180 
    pozicija=180;
  }

  motor.write(pozicija);                   //Postavi motor na poziciju varijable 
  if((motor.read() == 0) or (motor.read() == 180))   //Ledica je upaljena dok je servo u krajnjim pozicijama
  {  
    digitalWrite(LED, HIGH);
  } else
  {
    digitalWrite(LED, LOW);
  }
    zaslon.fillScreen(TFT_BLACK);                        //Pozadina crna
    zaslon.setCursor(30, 75);                            //Pozicioniraj kursor na ekranu
    zaslon.setTextColor(TFT_GREEN);                      //Postavi boju teksta u Zelenu
    zaslon.setTextFont(4);                               //Font velicine 4
    zaslon.println("Servo je u poziciji: "); 
    zaslon.setCursor(100, 100); 
    zaslon.println(motor.read());                         //Ispis teksta pozicije servo motora
  delay(50);                               //Pauza 50mS zbog stabilnosti
} 
