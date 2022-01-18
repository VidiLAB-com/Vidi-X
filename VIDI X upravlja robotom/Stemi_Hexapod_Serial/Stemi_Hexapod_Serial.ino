// Biblioteka koja vam je potrebna za korištenje Hexapoda nalazi se na linku
// https://github.com/stemi-education/stemi-hexapod
#include "Hexapod.h"

SharedData robot;
Hexapod  hexapod;

#define RXD2 4
#define TXD2 23

int n = 0;
int i = 1;

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  hexapod.init(ROBOT_USER_MODE);
  robot.setLed(GREEN);
  robot.setHeight(10);
}

Color clrArray[7] = {BLUE, YELLOW, GREEN, CYAN, PURPLE, RED, ORANGE};
uint8_t clrCount = 0;

void loop()
{
  if (Serial2.available()) //check incoming on default serial (USB) from PC
  {
    int a = Serial2.read();
    Serial.println(a); //Ispisujemo primljenu vrijednost kako bi vidjeli vrijednost koja treba stajati kod if uvjeta

    if ( a == 54)
    {
      robot.setHeight(50);
      n = 0;
    }
    else if (a == 53)
    {
      robot.setHeight(100);
      n = 1;
    }
    else if (a == 49)
    {
      robot.move(FORWARD, 2000);
    }
    else if (a == 50)
    {
      robot.move(BACKWARD, 2000);
    }
    else if (a == 51)
    {
      robot.move(LEFT, 2000);
    }
    else if (a == 52)
    {
      robot.move(RIGHT, 2000);
    }
    else if (a == 55)
    {
      robot.rotate(LEFT, 2000);
    }
    else if (a == 56)
    {
      robot.rotate(RIGHT, 2000);
    }

    setLED();
    delay(20);
  }
}

void setLED()
{
  if (n == 1)
  {
    i++;
    if ((i % 2) == 1) {
      robot.setLedStatic(0, RED);
      robot.setLedStatic(1, RED);
      robot.setLedStatic(2, RED);
      robot.setLedStatic(3, BLUE);
      robot.setLedStatic(4, BLUE);
      robot.setLedStatic(5, BLUE);
    } else
    {
      robot.setLedStatic(3, RED);
      robot.setLedStatic(4, RED);
      robot.setLedStatic(5, RED);
      robot.setLedStatic(0, BLUE);
      robot.setLedStatic(1, BLUE);
      robot.setLedStatic(2, BLUE);
    }
  } else
  {
    setLEDrandom();
  }
}

void setLEDrandom()
{
  robot.setLedStatic(0, clrArray[random(0, 6)]);
  robot.setLedStatic(1, clrArray[random(0, 6)]);
  robot.setLedStatic(2, clrArray[random(0, 6)]);
  robot.setLedStatic(3, clrArray[random(0, 6)]);
  robot.setLedStatic(4, clrArray[random(0, 6)]);
  robot.setLedStatic(5, clrArray[random(0, 6)]);
}
