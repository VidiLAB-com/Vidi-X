#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include <SPI.h>

#include <Wire.h>
#include <MCP23017.h>

#define MCP_ADDRESS 0x27 // (A2/A1/A0 = LOW)
//#define RESET_PIN 5
#define I2C_SDA 33
#define I2C_SCL 32

// ILI9341 TFT LCD pins
#define TFT_CS   5
#define TFT_DC  21

// Create TFT object
Adafruit_ILI9341 TFT = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Create myMCP object
MCP23017 myMCP = MCP23017(MCP_ADDRESS);
int wT = 1000; // wT = waiting time
byte portStatus;
bool pinStatus;

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL, 100000); // wake up I2C bus

  TFT.begin();
  TFT.setRotation(3);
  TFT.setTextSize(3);
  TFT.fillScreen(ILI9341_BLACK);
  TFT.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  TFT.println("    Dodavanje");
  TFT.println("  vanjskih GPIO");
  TFT.println(" pinova upotrebom");
  TFT.println(" MCP23017 modula!");
  
  if (!myMCP.Init()) {
    Serial.println("MCP23017 is not connected!");
    TFT.println("MCP23017 is not connected!");
    while (1) {}
  }
  myMCP.setPortMode(0b11111111, A);  // Port A: all pins are OUTPUT
  myMCP.setPortMode(0b00000000, B);  // Port B: all pins are INPUT
  delay(wT);
  myMCP.setAllPins(A, ON); // alle LEDs switched on
  delay(wT);
  myMCP.setPinX(1, A, OUTPUT, LOW); // A1 switched off
  delay(wT);
  myMCP.setPort(0b11110000, A); // A4 - A7 switched on
  delay(wT);
  myMCP.setPort(0b01011110, A); // A0,A5,A7 switched off
  delay(wT);
  myMCP.setPinX(0, A, OUTPUT, HIGH); // A0 switched on
  delay(wT);
  myMCP.setPinX(4, A, OUTPUT, LOW); // A4 switched off
  delay(wT);
  myMCP.setAllPins(A, HIGH); // A0 - A7 all on
  delay(wT);
  myMCP.setPin(3, A, LOW); // A3 switched off
  delay(wT);
  
  for (int i = 0; i < 5; i++) { // A0 and A1 blinking
    myMCP.togglePin(0, A);// A0 toggle on-off
    delay(100);
    myMCP.togglePin(1, A);// A1 toggle on-off
    delay(100);
  }
  for (int i = 0; i < 5; i++) { // A1 and A0 blinking
    myMCP.togglePin(1, A);
    delay(200);
    myMCP.togglePin(0, A);
    delay(200);
  }
}

void loop() {
  myMCP.togglePin(1, A); // continue A1 blinking
  delay(500);
  myMCP.togglePin(0, A); // continue A0 blinking
  delay(500);
 
  portStatus = myMCP.getPort(A); // get status at port A
  TFT.setTextColor(ILI9341_RED, ILI9341_BLACK);
  TFT.setCursor(0, 120);
  TFT.print("Status GPIO A: ");
  Serial.print("Status GPIO A: ");
  TFT.setCursor(10, 150);
  TFT.print(portStatus, BIN);
  TFT.print("        ");
  Serial.println(portStatus, BIN);
  
  portStatus = myMCP.getPort(B);// get status at port B
  TFT.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  TFT.setCursor(0, 180);
  TFT.print("Status GPIO B: ");
  Serial.print("Status GPIO B: ");
  TFT.setCursor(10, 210);
  TFT.print(portStatus, BIN);
  TFT.print("        ");
  Serial.println(portStatus, BIN);
  
  //TFT.println("-----------------------");
  Serial.println("-----------------------");
}
