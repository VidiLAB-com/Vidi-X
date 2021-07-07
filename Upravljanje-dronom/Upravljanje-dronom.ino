#include <WiFi.h>
#include <WiFiUDP.h>

WiFiUDP Udp;

#define WIFI "TELLO-598E30"
#define VRIJEME 30000
#define IPTELLO "192.168.10.1"
#define UDPPORT 8889

int i = 0;
int volume = 0;
int menu = 13;
int start = 39;
int selec = 27;
int btn_a = 32;
int btn_b = 33;
int btn_lir = 34;
int btn_uid = 35;

void setup() {
  Serial.begin(9600);
  spajanjeWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    Udp.begin(UDPPORT);
    slanjeNaredbe("command");
  }

  pinMode(btn_a, INPUT_PULLUP);
  pinMode(btn_b, INPUT_PULLUP);
  pinMode(btn_lir, INPUT_PULLUP);
  pinMode(btn_uid, INPUT_PULLUP);
  pinMode(selec, INPUT_PULLUP);
  pinMode(menu, INPUT_PULLUP);
  pinMode(start, INPUT_PULLUP);
  pinMode(volume, INPUT_PULLUP);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    if (digitalRead(start) == LOW) {
      slanjeNaredbe("takeoff");
      Serial.println("Tello uzlijece");
      delay(1000);
    }

    if (digitalRead(selec) == LOW) {
      slanjeNaredbe("land");
      Serial.println("Tello slijeće");
      delay(1000);
    }

    if (digitalRead(btn_a) == LOW) {
      slanjeNaredbe("up 31");
      Serial.println("Tello ide gore");
      delay(1000);
    }

    if (digitalRead(btn_b) == LOW) {
      slanjeNaredbe("down 31");
      Serial.println("Tello ide dolje");
      delay(1000);
    }

    if (analogRead(btn_lir) > 4000) {
      slanjeNaredbe("left 31");
      Serial.println("Tello ide ulijevo");
      delay(1000);
    }

    if (analogRead(btn_lir) > 1900 && analogRead(btn_lir) < 2000) {
      slanjeNaredbe("right 31");
      Serial.println("Tello ide udesno");
      delay(1000);
    }

    if (analogRead(btn_uid) > 4000) {
      slanjeNaredbe("forward 51");
      Serial.println("Tello ide naprijed");
      delay(1000);
    }

    if (analogRead(btn_uid) > 1900 && analogRead(btn_uid) < 2000) {
      slanjeNaredbe("back 51");
      Serial.println("Tello ide nazad");
      delay(1000);
    }

    if (digitalRead(btn_b) == LOW) {
      slanjeNaredbe("down 31");
      Serial.println("Tello ide dolje");
      delay(1000);
    }

    if (digitalRead(volume) == LOW) {
      slanjeNaredbe("ccw 45");
      Serial.println("Tello se rotira za 45 stupnjeva u suprotnom smjeru kazaljke na satu");
      delay(1000);
    }

    if (digitalRead(menu) == LOW) {
      slanjeNaredbe("cw 45");
      Serial.println("Tello se rotira za 45 stupnjeva u smjeru kazaljke na satu");
      delay(1000);
    }
  }
}

void spajanjeWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI);

  unsigned long vrijeme_pocetak = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - vrijeme_pocetak < VRIJEME) {
    Serial.println("Pokušava se spojiti na WiFi mrežu.");
    delay(2000);
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("VIDI X je spojen na WiFi mrežu.");
  } else {
    Serial.println("VIDI X se nije uspio spojiti na WiFi mrežu.");
  }
}

void slanjeNaredbe(String naredba) {
  Udp.beginPacket(IPTELLO, UDPPORT);
  Udp.write((const unsigned char*)naredba.c_str(), naredba.length());
  Udp.endPacket();
  if (i == 0) {
    Serial.println("Možete započeti slanje naredbi");
  }
  i++;
}
