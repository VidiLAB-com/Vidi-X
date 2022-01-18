
const int statusLED = 2;
const int UP_DOWN = 35; // Off = 0   UP =  Down =
const int LEFT_RIGHT = 34; // Off = 0   Left =  Right =
const int gumb_A = 32; // Off = 4095  On = 0
const int gumb_B = 33; //  Off = 4095  On = 0
const int gumb_Select = 27; //  Off = 4095  On = 0
const int gumb_Start = 39; //  Off = 4095  On = 0
const int gumb_Vol = 0; //  Off = 4095  On = 0
const int gumb_Menu = 13; //  Off = 3431 On = 0
const int tempAnalogInPin = 26; // Analogni input pin Senzora Temperature
float tempC1;
int v;
int n=40;

#define RXD2 13
#define TXD2 4

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  char poruka[32];
  //  int b;
  //  float c;
  //  bool d;
} struct_message;

// Create a struct_message called myData
struct_message myData;

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Hi:");
  pinMode(0, INPUT);
  pinMode(2, OUTPUT);
  pinMode(13, INPUT);
  pinMode(26, INPUT);
  pinMode(27, INPUT);
  pinMode(32, INPUT);
  pinMode(33, INPUT);
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  pinMode(39, INPUT);
  digitalWrite(statusLED, LOW);
}

float temperature(int analog) {
  double temp =
    analog * 3.30 / 4095.0; // Analognu vrijednost pretvaramo u otpor
  temp = temp - 0.40;         // Oduzimamo 500mA tj. -40°C
  temp = temp / 0.01;         // Pretvaramo vrijednosti u °C
  return temp;
}

void loop() {
  digitalWrite(statusLED, LOW);
  tempC1 = temperature(analogRead(tempAnalogInPin)); // Pozivamo funkciju za pretvorbu analognih
  // očitanja u stupnjeve Cezijeve

  if ( analogRead(UP_DOWN) == 4095 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("1"); /// Serial.read daje 49, 252
    Serial.println("1");
    //strcpy(myData.poruka, "gumb_UP");
    //Serial2.print(myData.poruka);
  }

  if ( analogRead(UP_DOWN) > 1700 && analogRead(UP_DOWN) < 2300 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("2");  /// Serial.read daje 50, 240
    Serial.println("2");
  }

  if ( analogRead(LEFT_RIGHT) == 4095 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("3");  /// Serial.read daje 51, 224
    Serial.println("3");
  }

  if ( analogRead(LEFT_RIGHT) > 1700 && analogRead(LEFT_RIGHT) < 2100 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("4");  /// Serial.read daje 52, 224, 240
    Serial.println("4");
  }

  if ( analogRead(gumb_A) == 0 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("5"); /// Serial.read daje 53, 128
    Serial.println("5");
  }


  if ( analogRead(gumb_B) == 0 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("6"); /// Serial.read daje 54,
    Serial.println("6");
  }

  if ( analogRead(gumb_Select) == 0 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("7"); /// Serial.read daje 55,
    Serial.println("7");
  }

  if ( analogRead(gumb_Start) == 0 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("8"); /// Serial.read daje 56,
    Serial.println("8");
    n++;
  }
  /*
      if ( analogRead(gumb_Vol) == 4095 ) { // || OR -
      Serial2.print("Bok Vol");
    }
    // POPRAVITI VRIJEDNOSTI
      if ( analogRead(gumb_Menu) == 3431 ) { // || OR - POPRAVITI VRIJEDNOSTI
    Serial.print(analogRead(gumb_Menu));
      Serial2.print("9");
    }
  */

  delay(500);
}
