/*potrebne biblioteke */
#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <gfxfont.h>
#include <Adafruit_ILI9341.h>

/*  SSID i lozinka za wifi mrežu */
const char* ssid = "VIDI-X-PROJECT";  // SSID
const char* password = "12345678";  // Lozinka

/* IP adresa servera */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

/*server sluša na portu 80 http*/
WebServer server(80);

/* Ekran pinovi */
#define TFT_DC 21
#define TFT_CS 5

/* Definicija boja */
#define WHITE    0xFFFF
#define BLACK    0x0000
#define GREEN    0x07E0

/*definicija PIN spojene LED-ice */
uint8_t LED1pin = 2;
bool LED1status = LOW;

/* Ekran biblioteka*/
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void setup() {
  Serial.begin(115200);
  pinMode(LED1pin, OUTPUT);
/* Begin TFT */
  tft.begin();
  
/*popunjavanje ekrana crnom bojom (fill screen)*/
  tft.fillScreen(BLACK);
  
/*rotacija ekrana*/
  tft.setRotation(3);
  
/*prikaz teksta na ekranu (IP address, Gateway, Subnet)*/
  tft.setCursor(10, 0);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.print("Server IP address: ");
  tft.println(local_ip);
  tft.setCursor(10, 10);
  tft.print("Gateway: ");
  tft.println(gateway);
  tft.setCursor(10, 20);
  tft.print("Subnet mask: ");
  tft.println(subnet);
  tft.setCursor(10, 100);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.println("VIDI Project X WEB server");

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
/*poziva funkciju kada klijent zahtjeva URI*/ 
  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.onNotFound(handle_NotFound);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 120);
  tft.println("Broj spojenih klijenata:");
  
  server.begin();
  Serial.println("HTTP server started");
}
void loop() {
  tft.setTextWrap(false);
  Serial.println("Broj spojenih klijenata:");
/*ispis broja spojenih klijenata*/
  Serial.println(WiFi.softAPgetStationNum());
  tft.setCursor(10, 130);
  tft.println(WiFi.softAPgetStationNum());
  delay(100);
  server.handleClient();
  if(LED1status)
  {digitalWrite(LED1pin, HIGH);}
  else
  {digitalWrite(LED1pin, LOW);}
}
void handle_OnConnect() {
  LED1status = LOW;
  Serial.println("GPIO2 Status: Isključena");
  server.send(200, "text/html", SendHTML(LED1status)); 
}
/* definicija kada je LED-ica upaljena*/
void handle_led1on() {
  LED1status = HIGH;
  Serial.println("GPIO2 Status: uključena");
  server.send(200, "text/html", SendHTML(true));
  tft.fillScreen(BLACK);
  tft.setCursor(10, 0);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.print("Server IP address: ");
  tft.println(local_ip);
  tft.setCursor(10, 10);
  tft.print("Gateway: ");
  tft.println(gateway);
  tft.setCursor(10, 20);
  tft.print("Subnet mask: ");
  tft.println(subnet);
  tft.setCursor(10, 100);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.println("VIDI Project X WEB server");
  tft.setTextSize(1);
  tft.setTextColor(GREEN);
  tft.setCursor(10, 150);
  tft.print("LED-ica upaljena"); 
}
/* definicija kada je LED-ica ugašena*/
void handle_led1off() {
  LED1status = LOW;
  Serial.println("GPIO2 Status: isključena");
  server.send(200, "text/html", SendHTML(false));
  tft.fillScreen(BLACK);
  tft.setCursor(10, 0);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.print("Server IP address: ");
  tft.println(local_ip);
  tft.setCursor(10, 10);
  tft.print("Gateway: ");
  tft.println(gateway);
  tft.setCursor(10, 20);
  tft.print("Subnet mask: ");
  tft.println(subnet);
  tft.setCursor(10, 100);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.println("VIDI Project X WEB server");
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(10, 150);
  tft.print("LED-ica ugasena");  
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
/*CSS i HTML stranice na serveru*/
String SendHTML(uint8_t led1stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Web Server</h1>\n";
  ptr +="<h3>VIDI Project X</h3>\n";
  
   if(led1stat)
  {ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
  else
  {ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
