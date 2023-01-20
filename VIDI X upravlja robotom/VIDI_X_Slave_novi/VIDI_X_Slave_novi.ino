#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include <SPI.h>

#define TFT_CS   5
#define TFT_DC  21

// Stvori objekt zaslona koji zovemo TFT
Adafruit_ILI9341 TFT = Adafruit_ILI9341(TFT_CS, TFT_DC);

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
int n = 40;
int poruka = 0;
boolean gore = false;

#define RXD2 13
#define TXD2 4

void initDisplay()
{
  TFT.begin();                   // inicijalizacuija zaslona
  TFT.setRotation(3);            // postavi orijentaciju
  TFT.setTextColor(ILI9341_RED);
  TFT.setTextSize(1);
  TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
}

void drawStartScreen()
{
  TFT.fillScreen(ILI9341_BLACK);

  //Draw white frame
  TFT.drawRect(0, 0, 319, 240, ILI9341_WHITE);

  TFT.setCursor(45, 20);
  TFT.setTextColor(ILI9341_RED);
  TFT.setTextSize(4);
  TFT.print("Resistance");

  //Print "VIDI X mikroracunalo" text
  TFT.setTextColor(ILI9341_GREEN);
  TFT.setTextSize(4);
  TFT.setCursor(140, 60);
  TFT.print("is");
  TFT.setCursor(95, 100);
  TFT.print("Futile");
}

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP() {
  const char *SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}


void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Hi:");
  pinMode(0, INPUT);
  pinMode(2, OUTPUT);
  pinMode(13, INPUT);
  pinMode(26, INPUT);
  pinMode(27, INPUT_PULLUP);
  pinMode(32, INPUT);
  pinMode(33, INPUT);
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  pinMode(39, INPUT);
  digitalWrite(statusLED, LOW);
  initDisplay();
  drawStartScreen();

  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
}

float temperature(int analog) {
  double temp =
    analog * 3.30 / 4095.0; // Analognu vrijednost pretvaramo u otpor
  temp = temp - 0.40;         // Oduzimamo 500mA tj. -40°C
  temp = temp / 0.01;         // Pretvaramo vrijednosti u °C
  return temp;
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  digitalWrite(statusLED, HIGH);
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  poruka = *data;
  //Serial.print("Last Packet Recv Data: "); Serial.println(poruka);
  //Serial.println("");
  digitalWrite(statusLED, LOW);
}

void loop() {

  digitalWrite(statusLED, LOW);
  //poruka = 0;
  tempC1 = temperature(analogRead(tempAnalogInPin)); // Pozivamo funkciju za pretvorbu analognih
  // očitanja u stupnjeve Cezijeve

  if ( poruka == 49 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("1"); /// Serial.read daje 49
    Serial.println("1");
    poruka = 0;
  }
  if ( analogRead(UP_DOWN) == 4095 ) { // || OR -
    /*digitalWrite(statusLED, HIGH);
      Serial2.println("1"); /// Serial.read daje 49
      Serial.println("1");
      poruka = 49;*/
    //strcpy(myData.poruka, "gumb_UP");
    //Serial2.print(myData.poruka);
  }
  if ( poruka == 50 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("2"); /// Serial.read daje 50
    Serial.println("2");
    poruka = 0;
  }
  if ( analogRead(UP_DOWN) > 1700 && analogRead(UP_DOWN) < 2300 ) { // || OR -
    /*digitalWrite(statusLED, HIGH);
      Serial2.println("2");  /// Serial.read daje 50
      Serial.println("2");
      poruka = 50;*/
  }
  if ( poruka == 51 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("3"); /// Serial.read daje 51
    Serial.println("3");
    poruka = 0;
  }
  if ( analogRead(LEFT_RIGHT) == 4095 ) { // || OR -
    /*digitalWrite(statusLED, HIGH);
      Serial2.println("3");  /// Serial.read daje 51
      Serial.println("3");
      poruka = 51;*/
  }
  if ( poruka == 52 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("4"); /// Serial.read daje 52
    Serial.println("4");
    poruka = 0;
  }
  if ( analogRead(LEFT_RIGHT) > 1700 && analogRead(LEFT_RIGHT) < 2100 ) { // || OR -
    /*digitalWrite(statusLED, HIGH);
      Serial2.println("4");  /// Serial.read daje 52
      Serial.println("4");
      poruka = 52;*/
  }
  if ( poruka == 53 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("5"); /// Serial.read daje 53
    Serial.println("5");
    poruka = 0;
    gore = true;
  }
  if ( analogRead(gumb_A) == 0 ) { // || OR -
    /*digitalWrite(statusLED, HIGH);
      Serial2.println("5"); /// Serial.read daje 53
      Serial.println("5");
      poruka = 53;*/
  }
  if ( poruka == 54 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("6"); /// Serial.read daje 54
    Serial.println("6");
    poruka = 0;
    gore = false;
  }
  if ( analogRead(gumb_B) == 0 ) { // || OR -
    /*digitalWrite(statusLED, HIGH);
      Serial2.println("6"); /// Serial.read daje 54,
      Serial.println("6-a");
      poruka = 0; //poruka = 54; */
  }
  if ( poruka == 55 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("6"); /// Serial.read daje 55
    Serial.println("6");
    poruka = 0;
  }
  if ( analogRead(gumb_Select) == 0 ) { // || OR -
    /*digitalWrite(statusLED, HIGH);
      Serial2.println("7"); /// Serial.read daje 55,
      Serial.print("7-a ");
      Serial.println(analogRead(gumb_Select));
      poruka = 0; //poruka = 55; */
  }
  if ( poruka == 56 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("8"); /// Serial.read daje 56
    Serial.println("8");
    poruka = 0;
  }
  if ( analogRead(gumb_Start) == 0 ) { // || OR -
    /*digitalWrite(statusLED, HIGH);
      Serial2.println("8"); /// Serial.read daje 56,
      Serial.println("8");
      poruka = 56;
      n++;*/
  }
  n++;
  if ( n > 52000 ) { // || OR -
    if ( gore == false ) {
      poruka = 53;
      Serial.println("53");
    } else {
      poruka = 54;
      Serial.println("54");
    }
    n = 0;
  }
  //delay(500);
}
