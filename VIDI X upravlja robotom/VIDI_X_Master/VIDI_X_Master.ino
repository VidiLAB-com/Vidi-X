#include <esp_now.h>
#include <WiFi.h>

#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include <SPI.h>

// Global copy of slave
#define NUMSLAVES 20
esp_now_peer_info_t slaves[NUMSLAVES] = {};
int SlaveCnt = 0;

#define CHANNEL 3
#define PRINTSCANRESULTS 0


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

#define RXD2 13
#define TXD2 4

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

// Scan for slaves in AP mode
void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks();
  //reset slaves
  memset(slaves, 0, sizeof(slaves));
  SlaveCnt = 0;
  Serial.println("");
  if (scanResults == 0) {
    Serial.println("No WiFi devices in AP Mode found");
  } else {
    Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);

      if (PRINTSCANRESULTS) {
        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
      }
      delay(10);
      // Check if the current device starts with `Slave`
      if (SSID.indexOf("Slave") == 0) {
        // SSID of interest
        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
        // Get BSSID => Mac Address of the Slave
        int mac[6];

        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
          for (int ii = 0; ii < 6; ++ii ) {
            slaves[SlaveCnt].peer_addr[ii] = (uint8_t) mac[ii];
          }
        }
        slaves[SlaveCnt].channel = CHANNEL; // pick a channel
        slaves[SlaveCnt].encrypt = 0; // no encryption
        SlaveCnt++;
      }
    }
  }

  if (SlaveCnt > 0) {
    Serial.print(SlaveCnt); Serial.println(" Slave(s) found, processing..");
  } else {
    Serial.println("No Slave Found, trying again.");
  }

  // clean up ram
  WiFi.scanDelete();
}

// Check if the slave is already paired with the master.
// If not, pair the slave with master
void manageSlave() {
  if (SlaveCnt > 0) {
    for (int i = 0; i < SlaveCnt; i++) {
      Serial.print("Processing: ");
      for (int ii = 0; ii < 6; ++ii ) {
        Serial.print((uint8_t) slaves[i].peer_addr[ii], HEX);
        if (ii != 5) Serial.print(":");
      }
      Serial.print(" Status: ");
      // check if the peer exists
      bool exists = esp_now_is_peer_exist(slaves[i].peer_addr);
      if (exists) {
        // Slave already paired.
        Serial.println("Already Paired");
      } else {
        // Slave not paired, attempt pair
        esp_err_t addStatus = esp_now_add_peer(&slaves[i]);
        if (addStatus == ESP_OK) {
          // Pair success
          Serial.println("Pair success");
        } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
          // How did we get so far!!
          Serial.println("ESPNOW Not Init");
        } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
          Serial.println("Add Peer - Invalid Argument");
        } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
          Serial.println("Peer list full");
        } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
          Serial.println("Out of memory");
        } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
          Serial.println("Peer Exists");
        } else {
          Serial.println("Not sure what happened");
        }
        delay(100);
      }
    }
  } else {
    // No slave found to process
    Serial.println("No Slave found to process");
  }
}

uint8_t data = 0;
uint8_t poruka = 0;

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

  TFT.setTextColor(ILI9341_YELLOW);
  TFT.setTextSize(4);
  TFT.setCursor(95, 170);
  TFT.print("VIDI X");
  TFT.setCursor(40, 200);
  TFT.setTextColor(ILI9341_YELLOW);
  TFT.setTextSize(3);
  TFT.print("MIKRORACUNALO");
}

// send data
void sendData() {
  //data++;
  data = poruka;
  for (int i = 0; i < SlaveCnt; i++) {
    const uint8_t *peer_addr = slaves[i].peer_addr;
    if (i == 0) { // print only for first slave
      Serial.print("Sending: ");
      Serial.println(data);
    }
    esp_err_t result = esp_now_send(peer_addr, &data, sizeof(data));
    Serial.print("Send Status: ");
    if (result == ESP_OK) {
      Serial.println("Success");
    } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
      // How did we get so far!!
      Serial.println("ESPNOW not Init.");
    } else if (result == ESP_ERR_ESPNOW_ARG) {
      Serial.println("Invalid Argument");
    } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
      Serial.println("Internal Error");
    } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
      Serial.println("ESP_ERR_ESPNOW_NO_MEM");
    } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
      Serial.println("Peer not found.");
    } else {
      Serial.println("Not sure what happened");
    }
    delay(100);
  }
}

// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

float temperature(int analog) {
  double temp =
    analog * 3.30 / 4095.0; // Analognu vrijednost pretvaramo u otpor
  temp = temp - 0.40;         // Oduzimamo 500mA tj. -40°C
  temp = temp / 0.01;         // Pretvaramo vrijednosti u °C
  return temp;
}

void upravljanje() {
  if ( analogRead(UP_DOWN) == 4095 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("1"); /// Serial.read daje 49
    Serial.println("1");
    poruka = 49;
    //strcpy(myData.poruka, "gumb_UP");
    //Serial2.print(myData.poruka);
  }
  if ( analogRead(UP_DOWN) > 1700 && analogRead(UP_DOWN) < 2300 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("2");  /// Serial.read daje 50
    Serial.println("2");
    poruka = 50;
  }
  if ( analogRead(LEFT_RIGHT) == 4095 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("3");  /// Serial.read daje 51
    Serial.println("3");
    poruka = 51;
  }
  if ( analogRead(LEFT_RIGHT) > 1700 && analogRead(LEFT_RIGHT) < 2100 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("4");  /// Serial.read daje 52
    Serial.println("4");
    poruka = 52;
  }
  if ( analogRead(gumb_A) == 0 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("5"); /// Serial.read daje 53
    Serial.println("5");
    poruka = 53;
  }
  if ( analogRead(gumb_B) == 0 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("6"); /// Serial.read daje 54,
    Serial.println("6");
    poruka = 54;
  }
  if ( analogRead(gumb_Select) == 0 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("7"); /// Serial.read daje 55,
    Serial.println("7");
    poruka = 55;
  }
  if ( analogRead(gumb_Start) == 0 ) { // || OR -
    digitalWrite(statusLED, HIGH);
    Serial2.println("8"); /// Serial.read daje 56,
    Serial.println("8");
    poruka = 56;
    n++;
  }
}
void setup() {
  Serial.begin(115200);  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Hi:");
  // Init Serial Monitor

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
  initDisplay();
  drawStartScreen();

  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  Serial.println("ESPNow/Multi-Slave/Master Example");
  // This is the mac address of the Master in Station Mode
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
}

void loop() {
  // If Slave is found, it would be populate in `slave` variable
  // We will check if `slave` is defined and then we proceed further
  digitalWrite(statusLED, LOW);
  poruka = 0;
  tempC1 = temperature(analogRead(tempAnalogInPin)); // Pozivamo funkciju za pretvorbu analognih
                                                     // očitanja u stupnjeve Cezijeve
  if (SlaveCnt > 0) { // check if slave channel is defined
    // `slave` is defined
    // Add slave as peer if it has not been added already
    manageSlave();
    // pair success or already paired
    // Send data to device
    upravljanje();
    sendData();
  } else {
    // No slave found to process
    ScanForSlave();
  }
  // wait for 3seconds to run the logic again
  //delay(1000);
}
