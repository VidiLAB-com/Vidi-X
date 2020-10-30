#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

/* Display */
#define TFT_DC 21
#define TFT_CS 5

/* color definition */
#define WHITE  0xFFFF
#define BLACK  0x0000

/* Display library*/
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

TaskHandle_t Task1;
TaskHandle_t Task2;

// LED pins
const int led1 = 2;
const int led2 = 2; // moguće zamjeniti drugom LED-icom na drugom PIN-u

void setup() {
tft.begin();
tft.setRotation(3);
tft.fillScreen(0);
Serial.begin(115200);
pinMode(led1, OUTPUT);
pinMode(led2, OUTPUT);

  //kreira zadatak koji će biti izvršen u Task1code() funkciji, sa prioritetom 1 i izvršenjem na jezgri 0
  xTaskCreatePinnedToCore(
                    Task1code,   
                    "Task1",     
                    10000,       
                    NULL,        
                    1,           
                    &Task1,      
                    0);                           
  delay(500); 

  //kreira zadatak koji će biti izvršen u Task2code() funkciji, sa prioritetom 1 i izvršenjem na jezgri 1
  xTaskCreatePinnedToCore(
                    Task2code,   
                    "Task2",     
                    10000,       
                    NULL,        
                    1,           
                    &Task2,      
                    1);          
    delay(500); 
}

//Task1: LED-ICA blinka svakih 1000 ms
void Task1code( void * pvParameters ){
  Serial.print("Zadatak 1 izvrsava se na jezgri ");
  Serial.println(xPortGetCoreID());
tft.setCursor(10, 0);
tft.setTextSize(1);
tft.println("Zadatak 1 izvrsava se na jezgri ");
tft.setCursor(10, 10);
tft.println(xPortGetCoreID());

  for(;;){
    digitalWrite(led1, HIGH);
    delay(1000);
    digitalWrite(led1, LOW);
    delay(1000);
  } 
}

//Task2: LED-ICA blinka svakih 700 ms
void Task2code( void * pvParameters ){
  Serial.print("Zadatak 2 izvrsava se na jezgri ");
  Serial.println(xPortGetCoreID());
  tft.setTextSize(1);
  tft.setCursor(10, 20);
  tft.println("Zadatak 2 izvrsava se na jezgri ");
  tft.setCursor(10, 30);
  tft.println(xPortGetCoreID());
   
  for(;;){
    digitalWrite(led2, HIGH);
    delay(700);
    digitalWrite(led2, LOW);
    delay(700);
  }
}

void loop() {
  
}
