//////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////17.12.2020.////////////////////////////////////
////////IZRADIO://////IVOR///S.///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////Pritiskim na tipku A s Project X plocice//////////////////////////////////////
////Ukljucuje se relej i trosilo spojeno na njega/////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
  
#define Tipka_A 32							        // Tipka A na pločici
#define Relej 2								          // Relej spojite na pin 5, pin 2 koristimo
                                        // kao demo za upravljanje plavom LED-icom 
void setup() {
	pinMode(Tipka_A, INPUT_PULLUP);			  // Tipka A definirana kao ulaz
	pinMode(Relej, OUTPUT);					      // Relej definiran kao izlaz
}

void loop() {
	if(digitalRead(Tipka_A)) {            // Ocitavanje stanja tipke A
		digitalWrite(Relej, LOW);           // Ako je tipka pritisnuta    
	}                                     // Ukljucen relej
  else {                                // Ako nije pritisnuta tipka
    digitalWrite(Relej, HIGH);          // Relej iskljucen
  }
}
