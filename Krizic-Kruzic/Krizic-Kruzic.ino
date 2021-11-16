#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include <SPI.h>
#include <XPT2046_Touchscreen.h>

// ILI9341 TFT LCD deklaracija spajanja zaslona
#define TFT_CS   5
#define TFT_DC  21

// varijable za dimenzije zaslona
int myWidth;
int myHeight;
int j = 0;

// Stvori objekt zaslona koji zovemo TFT
Adafruit_ILI9341 TFT = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define TS_CS 4 // deklaracija pina za senzor dodira

// stvaranje objekta za senzor dodira koji će se zvati ts
XPT2046_Touchscreen ts(TS_CS);

#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000
#define Z_THRESHOLD     400

//#define YP A2  // must be an analog pin, use "An" notation!
//#define XM A3  // must be an analog pin, use "An" notation!
//#define YM 8   // can be a digital pin
//#define XP 9   // can be a digital pin

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

extern uint8_t circle[];
extern uint8_t x_bitmap[];
int gameScreen = 1;
int moves = 1;
int winner = 0;  //0 = Draw, 1 = Human, 2 = CPU
boolean buttonEnabled = true;
int board[] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // holds position data 0 is blank, 1 human, 2 is computer

int PlayerMove = 1;

void initDisplay()
{
  TFT.begin();                   // inicijalizacuija zaslona
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  ts.setRotation(3);            // postavi orijentaciju
  Serial.println("Touchscreen started");
  TFT.setRotation(3);            // postavi orijentaciju
  TFT.setTextColor(ILI9341_RED);
  TFT.setTextSize(1);
  myWidth  = TFT.width() ;       // ekran je širok
  myHeight = TFT.height();       // ekran je visok
  TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
}

void drawStartScreen()
{
  TFT.fillScreen(BLACK);

  //Draw white frame
  TFT.drawRect(0, 0, 319, 240, WHITE);

  //Print "Krizic Kruzic" Text
  TFT.setCursor(5, 120);
  TFT.setTextColor(WHITE);
  TFT.setTextSize(4);
  TFT.print("Krizic Kruzic");

  //Print "VIDI X mikroracunalo" text
  TFT.setTextColor(GREEN);
  TFT.setTextSize(4);
  TFT.setCursor(90, 30);
  TFT.print("VIDI X");
  TFT.setCursor(5, 60);
  TFT.print("mikroracunalo");

  createStartButton();
}

void createStartButton()
{
  //Create Red Button
  TFT.fillRect(60, 180, 200, 40, RED);
  TFT.drawRect(60, 180, 200, 40, WHITE);
  TFT.setCursor(72, 188);
  TFT.setTextColor(WHITE);
  TFT.setTextSize(3);
  TFT.print("Start Game");
}

void setup() {
  Serial.begin(115200);
  Serial.print("Starting...");
  randomSeed(analogRead(0));

  initDisplay();
  drawStartScreen();
}

void loop()
{
  TS_Point p = ts.getPoint();  //Get touch point

  if (gameScreen == 3)
  {
    buttonEnabled = true;
  }

  if (p.z > Z_THRESHOLD) {

    p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
    p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);
    Serial.print("Z = "); Serial.print(p.z);
    Serial.print("\tX = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print("\n");

    if (p.x > 60 && p.x < 260 && p.y > 180 && p.y < 220 && buttonEnabled) // The user has pressed inside the red rectangle
    {
      buttonEnabled = false; //Disable button
      Serial.println("Button Pressed");
      resetGame();
      //This is important, because the libraries are sharing pins
      //pinMode(XM, OUTPUT);
      //pinMode(YP, OUTPUT);

      drawGameScreen();
      playGame();

    }
    delay(10);
  }
}

void resetGame()
{
  buttonEnabled = false;
  for (int i = 0; i < 9; i++)
  {
    board[i] = 0;
  }
  moves = 1;
  winner = 0;
  gameScreen = 2;
}

void drawGameScreen()
{
  TFT.fillScreen(BLACK);

  //Draw frame
  TFT.drawRect(0, 0, 319, 240, WHITE);

  drawVerticalLine(125);
  drawVerticalLine(195);
  drawHorizontalLine(80);
  drawHorizontalLine(150);
}

void drawGameOverScreen()
{

  TFT.fillScreen(BLACK);

  //Draw frame
  TFT.drawRect(0, 0, 319, 240, WHITE);

  //Print "Game Over" Text
  TFT.setCursor(50, 30);
  TFT.setTextColor(WHITE);
  TFT.setTextSize(4);
  TFT.print("GAME OVER");

  if (winner == 0)
  {
    //Print "DRAW!" text
    TFT.setCursor(110, 100);
    TFT.setTextColor(YELLOW);
    TFT.setTextSize(4);
    TFT.print("DRAW");
  }
  if (winner == 1)
  {
    //Print "HUMAN WINS!" text
    TFT.setCursor(40, 100);
    TFT.setTextColor(BLUE);
    TFT.setTextSize(4);
    TFT.print("HUMAN WINS");
  }

  if (winner == 2)
  {
    //Print "CPU WINS!" text
    TFT.setCursor(60, 100);
    TFT.setTextColor(RED);
    TFT.setTextSize(4);
    TFT.print("CPU WINS");
  }

  createPlayAgainButton();

  //pinMode(XM, OUTPUT);
  //pinMode(YP, OUTPUT);
}

void createPlayAgainButton()
{
  //Create Red Button
  TFT.fillRect(60, 180, 200, 40, RED);
  TFT.drawRect(60, 180, 200, 40, WHITE);
  TFT.setCursor(72, 188);
  TFT.setTextColor(WHITE);
  TFT.setTextSize(3);
  TFT.print("Play Again");
}

void drawHorizontalLine(int y)
{
  int i = 0;
  for (i = 0; i < 7; i++)
  {
    TFT.drawLine(60, y + i, 270, y + i, WHITE);
  }
}

void drawVerticalLine(int x)
{
  int i = 0;
  for (i = 0; i < 7; i++)
  {
    TFT.drawLine(x + i, 20, x + i, 220, WHITE);
  }
}

void playGame()
{
  do
  {
    if (moves % 2 == 0) //Prvi je Player (moves % 2 == 0) ili CPU ako je (moves % 2 == 1)
    {
      computerMove();             //Serial.println("253 computerMove");
      printBoard();               //Serial.println("254 printBoard");
      checkWinner();              //Serial.println("255 checkWinner");
    } else
    {
      playerMove();               //Serial.println("258 playerMove");
      printBoard();               //Serial.println("259 printBoard");
      checkWinner();              //Serial.println("260 checkWinner");
    }
    moves++; //Serial.println(moves);
  }
  while (winner == 0 && moves < 10);
  if (winner == 1)
  {
    Serial.println("HUMAN WINS");
    delay(3000);
    gameScreen = 3;
    drawGameOverScreen();
  } else if (winner == 2)
  {
    Serial.println("CPU WINS");
    delay(3000);
    gameScreen = 3;
    drawGameOverScreen();
  } else
  {
    Serial.println("DRAW");
    delay(3000);
    gameScreen = 3;
    drawGameOverScreen();
  }

}

void playerMove()
{
  //pinMode(XM, OUTPUT);
  //pinMode(YP, OUTPUT);
  TS_Point p;
  boolean validMove = false;
  Serial.print("\nPlayer Move:");

  do
  {
    p = ts.getPoint();  //Get touch point
    if (p.z > Z_THRESHOLD  && validMove == false )
    {
      p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
      p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);
      Serial.print("Z = "); Serial.print(p.z);
      Serial.print("\tX = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\n");
      PlayerMove = 0;

      if ((p.x < 115) && (p.y >= 150)) //6
      {
        if (board[6] == 0)
        {
          Serial.println("Player Move: 6");
          PlayerMove = 1;
          validMove = true;
          board[6] = 1;
          drawPlayerMove(6);
          Serial.println("Drawing player move");
        }
      }
      else if ((p.x > 0 && p.x < 115) && (p.y < 150 && p.y > 80)) //3
      {

        if (board[3] == 0)
        {
          Serial.println("Player Move: 3");
          PlayerMove = 1;
          validMove = true;
          board[3] = 1;
          drawPlayerMove(3);
          Serial.println("Drawing player move");
        }
      }
      else if ((p.x < 125) && (p.y < 80)) //0
      {
        if (board[0] == 0)
        {
          Serial.println("Player Move: 0");
          PlayerMove = 1;
          validMove = true;
          board[0] = 1;
          drawPlayerMove(0);
        }
      }

      else if ((p.x > 125 && p.x <= 195) && (p.y < 80)) //1
      {
        if (board[1] == 0)
        {
          Serial.println("Player Move: 1");
          PlayerMove = 1;
          validMove = true;
          board[1] = 1;
          drawPlayerMove(1);
        }
      }

      else if ((p.x > 195) && (p.y < 80)) //2
      {
        if (board[2] == 0)
        {
          Serial.println("Player Move: 2");
          PlayerMove = 1;
          validMove = true;
          board[2] = 1;
          drawPlayerMove(2);
        }
      }

      else if ((p.x > 125 && p.x <= 195) && (p.y < 150 && p.y > 80)) //4
      {
        if (board[4] == 0)
        {
          Serial.println("Player Move: 4");
          PlayerMove = 1;
          validMove = true;
          board[4] = 1;
          drawPlayerMove(4);
        }
      }

      else if ((p.x > 195) && (p.y < 150 && p.y > 80)) //5
      {
        if (board[5] == 0)
        {
          Serial.println("Player Move: 5");
          PlayerMove = 1;
          validMove = true;
          board[5] = 1;
          drawPlayerMove(5);
        }
      }

      else if ((p.x > 125 && p.x <= 195) && (p.y > 150)) //7
      {
        if (board[7] == 0)
        {
          Serial.println("Player Move: 7");
          PlayerMove = 1;
          validMove = true;
          board[7] = 1;
          drawPlayerMove(7);
        }
      }

      else if ((p.x > 195) && (p.y > 150)) //8
      {
        if (board[8] == 0)
        {
          Serial.println("Player Move: 8");
          PlayerMove = 1;
          validMove = true;
          board[8] = 1;
          drawPlayerMove(8);
        }
      }
    }
  } while ( p.z < Z_THRESHOLD );
}

void printBoard()
{
  int i = 0;
  Serial.println("Board: [");
  for (i = 0; i < 9; i++)
  {
    Serial.print(board[i]);
    Serial.print(",");
  }
  Serial.print("]");
}

int checkOpponent()
{
  if (board[0] == 1 && board[1] == 1 && board[2] == 0)
    return 2;
  else if (board[0] == 1 && board[1] == 0 && board[2] == 1)
    return 1;
  else if (board[1] == 1 && board [2] == 1 && board[0] == 0)
    return 0;
  else if (board[3] == 1 && board[4] == 1 && board[5] == 0)
    return 5;
  else if (board[4] == 1 && board[5] == 1 && board[3] == 0)
    return 3;
  else if (board[3] == 1 && board[4] == 0 && board[5] == 1)
    return 4;
  else if (board[1] == 0 && board[4] == 1 && board[7] == 1)
    return 1;
  else
    return 100;
}

void computerMove()
{
  int b = 0;
  int counter = 0;
  int movesPlayed = 0;
  Serial.print("\nArduino Move:");

  int firstMoves[] = {0, 2, 6, 8}; // will use these positions first

  for (counter = 0; counter < 4; counter++) //Count first moves played
  {
    if (board[firstMoves[counter]] != 0) // First move is played by someone
    {
      movesPlayed++;
    }
  }
  do {
    if (moves <= 2)
    {
      int randomMove = random(4);
      int c = firstMoves[randomMove];

      if (board[c] == 0)
      {
        delay(1000);
        board[c] = 2;
        Serial.print(firstMoves[randomMove]);
        Serial.println();
        drawCpuMove(firstMoves[randomMove]);
        b = 1;
      }

    } else
    {
      int nextMove = checkOpponent();
      if (nextMove == 100)
      {
        if (movesPlayed == 4) //All first moves are played
        {
          int randomMove = random(9);
          if (board[randomMove] == 0)
          {
            delay(1000);
            board[randomMove] = 2;
            Serial.print(randomMove);
            Serial.println();
            drawCpuMove(randomMove);
            b = 1;
          }
        } else
        {
          int randomMove = random(4);
          int c = firstMoves[randomMove];

          if (board[c] == 0)
          {
            delay(1000);
            board[c] = 2;
            Serial.print(firstMoves[randomMove]);
            Serial.println();
            drawCpuMove(firstMoves[randomMove]);
            b = 1;
          }
        }
      } else
      {
        delay(1000);
        board[nextMove] = 2;
        drawCpuMove(nextMove);
        b = 1;
      }
    }
  }
  while (b < 1);
}

void drawCircle(int x, int y)
{ drawBitmap(x, y, circle, 65, 65, RED);
}

void drawX(int x, int y)
{
  drawBitmap(x, y, x_bitmap, 65, 65, BLUE);
}

void drawCpuMove(int move)
{
  switch (move)
  {
    case 0: drawCircle(55, 15);  break;
    case 1: drawCircle(130, 15); break;
    case 2: drawCircle(205, 15); break;
    case 3: drawCircle(55, 85);  break;
    case 4: drawCircle(130, 85); break;
    case 5: drawCircle(205, 85); break;
    case 6: drawCircle(55, 155); break;
    case 7: drawCircle(130, 155); break;
    case 8: drawCircle(205, 155); break;
  }
}

void drawPlayerMove(int move)
{
  switch (move)
  {
    case 0: drawX(55, 15);  break;
    case 1: drawX(130, 15); break;
    case 2: drawX(205, 15); break;
    case 3: drawX(55, 85);  break;
    case 4: drawX(130, 85); break;
    case 5: drawX(205, 85); break;
    case 6: drawX(55, 155); break;
    case 7: drawX(130, 155); break;
    case 8: drawX(205, 155); break;
  }
}

void checkWinner()
// checks board to see if there is a winner
// places result in the global variable 'winner'
{
  int qq = 0;
  // noughts win?
  if (board[0] == 1 && board[1] == 1 && board[2] == 1) {
    winner = 1;
  }
  if (board[3] == 1 && board[4] == 1 && board[5] == 1) {
    winner = 1;
  }
  if (board[6] == 1 && board[7] == 1 && board[8] == 1) {
    winner = 1;
  }
  if (board[0] == 1 && board[3] == 1 && board[6] == 1) {
    winner = 1;
  }
  if (board[1] == 1 && board[4] == 1 && board[7] == 1) {
    winner = 1;
  }
  if (board[2] == 1 && board[5] == 1 && board[8] == 1) {
    winner = 1;
  }
  if (board[0] == 1 && board[4] == 1 && board[8] == 1) {
    winner = 1;
  }
  if (board[2] == 1 && board[4] == 1 && board[6] == 1) {
    winner = 1;
  }
  // crosses win?
  if (board[0] == 2 && board[1] == 2 && board[2] == 2) {
    winner = 2;
  }
  if (board[3] == 2 && board[4] == 2 && board[5] == 2) {
    winner = 2;
  }
  if (board[6] == 2 && board[7] == 2 && board[8] == 2) {
    winner = 2;
  }
  if (board[0] == 2 && board[3] == 2 && board[6] == 2) {
    winner = 2;
  }
  if (board[1] == 2 && board[4] == 2 && board[7] == 2) {
    winner = 2;
  }
  if (board[2] == 2 && board[5] == 2 && board[8] == 2) {
    winner = 2;
  }
  if (board[0] == 2 && board[4] == 2 && board[8] == 2) {
    winner = 2;
  }
  if (board[2] == 2 && board[4] == 2 && board[6] == 2) {
    winner = 2;
  }

}
void drawBitmap(int16_t x, int16_t y,
                const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;

  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (i & 7) byte <<= 1;
      else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if (byte & 0x80) TFT.drawPixel(x + i, y + j, color);
    }
  }
}