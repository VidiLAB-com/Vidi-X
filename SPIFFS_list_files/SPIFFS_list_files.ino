#include <SPIFFS.h> /* bibilioteka SPIFFS datotečnog sustava */
 
void setup() {
 
  Serial.begin(115200);
 
  if (!SPIFFS.begin(true)) {
    Serial.println("Greška u mountanju SPIFFS");
    return;
  }
 
  File root = SPIFFS.open("/"); /* Otvori root */
 
  File file = root.openNextFile(); /* slijedeći file */
 
  while(file){
 
      Serial.print("FILE: ");
      Serial.println(file.name());/* ispis datoteka */
 
      file = root.openNextFile(); /* ispis datoteka dok god ih ima */
  }
}
 
void loop() {}
