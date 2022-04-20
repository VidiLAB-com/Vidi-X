// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include <SPI.h>

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
// ILI9341 TFT LCD pins
#define TFT_CS   5
#define TFT_DC  21

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
// Replace with your network credentials
const char* ssid = "";
const char* password = "";

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
// Define GPIO numbers
int gpioNo3 = 2;

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
// Set your Static IP address
IPAddress local_IP(192, 168, 1, 184);

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
// screen dimensions
int myWidth;
int myHeight;

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
// Create TFT object
Adafruit_ILI9341 TFT = Adafruit_ILI9341(TFT_CS, TFT_DC);


/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
// send back buuton to browser to reload the last visited page on your laptop or smartphone
const char back_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>

<script>
  window.history.back();
</script>

</body>
</html>
)=====";

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
// main function for voice parsing - if this do that
void parseVoiceCommand(String text){
  TFT.print("Rekao si :");
      TFT.println(text);
    if ((text == "svjetlo") || (text == "Svjetlo.")) {
      //do something when var equals 
      digitalWrite(gpioNo3, !digitalRead(gpioNo3));   
      TFT.println("Mijenjam stanje svjetla!");
    }
    else if ((text == "kuhinja") || (text == "Kuhinja.")) {
      digitalWrite(13, !digitalRead(13));
      TFT.println("Mijenjam stanje svjetla u kuhinji!");
      }
    else if ((text == "dnevni boravak") || (text == "Dnevni boravak.")) {
      digitalWrite(14, !digitalRead(14));
      TFT.println("Mijenjam stanje svjetla u dnevnom boravku!");
      }
    else if ((text == "spavaća soba") || (text == "Spavaća soba.")) {
      digitalWrite(27, !digitalRead(27));
      TFT.println("Mijenjam stanje svjetla u spavaćoja sobi!");
      }
    else if ((text == "hodnik") || (text == "Hodnik.")) {
      digitalWrite(4, !digitalRead(4));
      TFT.println("Mijenjam stanje svjetla u hodniku!");
    } else {
      TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
      TFT.println("Nisam razumio!");
      TFT.println(text);
      }
}

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(4, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(27, OUTPUT);

  pinMode(gpioNo3, OUTPUT);

  TFT.begin();
  TFT.setRotation(3);
  myWidth  = TFT.width();
  myHeight = TFT.height();
  TFT.fillScreen(ILI9341_BLACK);
  TFT.setTextColor(ILI9341_GREEN);
  TFT.println("          Pametni dom!");
  TFT.println("Glasovno upravljanje pametnim domom!");

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
    TFT.println("STA Failed to configure");
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  TFT.println(WiFi.localIP());
 
  // Route for root / web page
  server.on("/stt", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if (request->hasParam("text")) {
      String text=request->getParam("text")->value();
      Serial.print("text="); Serial.println(text);
      parseVoiceCommand(text);
    }

    String s = back_page; //Read HTML contents
    request->send(200, "text/html", s); 
  });
      
  // Start server
  server.begin();
}

/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
void loop(){
}
