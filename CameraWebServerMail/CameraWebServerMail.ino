#include "esp_camera.h"
#include "SPI.h"
#include "driver/rtc_io.h"
#include <ESP_Mail_Client.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <TFT_eSPI.h>      // Hardware-specific library
TFT_eSPI TFT = TFT_eSPI(); // Stvori objekt zaslona koji zovemo TFT
const int analogInPin1 = 14;   // Analogni input pin

// varijable za dimenzije zaslona
int myWidth;
int myHeight;
int j = 0;
boolean POSLAN = 0;
int Timer = 0;

// Playing a digital WAV recording repeatadly using the XTronical DAC Audio library
// prints out to the serial monitor numbers counting up showing that the sound plays
// independently of the main loop
// See www.xtronical.com for write ups on sound, the hardware required and how to make
// the wav files and include them in your code
#include "SoundData.h"
#include "XT_DAC_Audio.h"

const int TrajanjeZvuka = 3110; // Promijenite li .wav zapis i ovu varijablu morate povećati ili smanjtit ovisno o trajanju vaše snimke
const int prag = 400; // Osjtljivost mikrofona, manje je osjetljivije

XT_Wav_Class DobarPasWav(dobarpas);     // create an object of type XT_Wav_Class that is used by
// the dac audio class (below), passing wav data as parameter.

XT_DAC_Audio_Class DacAudio(25, 0);   // Create the main player class object.
// Use GPIO 25, one of the 2 DAC pins and timer 0

uint32_t DemoCounter = 0;             // Just a counter to use in the serial monitor
// u ovom slučaju potrebna kao kratka pauza poterbna za sviranje zvuka.

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//

// Select camera model
#define CAMERA_MODEL_VIDI_X // Has PSRAM
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM

#include "camera_pins.h"

const char* ssid = "wifi ime mreže na koju se spajate";
const char* password = "wifi password";

void startCameraServer();

/** The smtp host name e.g. smtp.gmail.com for GMail or smtp.office365.com for Outlook or smtp.mail.yahoo.com
   For yahoo mail, log in to your yahoo mail in web browser and generate app password by go to
   https://login.yahoo.com/account/security/app-passwords/add/confirm?src=noSrc
   and use the app password as password with your yahoo mail account to login.
   The google app password signin is also available https://support.google.com/mail/answer/185833?hl=en
*/
#define SMTP_HOST "smtp.gmail.com"

/** The smtp port e.g.
   25  or esp_mail_smtp_port_25
   465 or esp_mail_smtp_port_465
   587 or esp_mail_smtp_port_587
*/
#define SMTP_PORT esp_mail_smtp_port_587

/* The log in credentials */
#define AUTHOR_EMAIL "testni.mail.za.slanje@gmail.com"
#define AUTHOR_PASSWORD "gmail password"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

// Photo File Name to save in SPIFFS
#define FILE_PHOTO "/photo.jpg"

camera_config_t config;

void Play() {
  DemoCounter = 0;
  while (DemoCounter < TrajanjeZvuka) {   // Ovdje definiramo trajanje zvuka, povećamo li varijablu zvuk će se ponoviti, smanjimo li ju zvuk će biti odrezan
    DacAudio.FillBuffer();                // Fill the sound buffer with data
    if (DobarPasWav.Playing == false)    // if not playing,
      DacAudio.Play(&DobarPasWav);       // play it, this will cause it to repeat and repeat...
    Serial.println(DemoCounter++);        // Zvuk se neće ćuti ukoliko ovdije nema barem jedna linija koda
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  pinMode(analogInPin1, INPUT);   // Postavi pin senzora kao INPUT

  TFT.begin();                   // inicijalizacuija zaslona
  TFT.setRotation(3);            // postavi orijentaciju
  myWidth  = TFT.width() ;       // ekran je širok
  myHeight = TFT.height();       // ekran je visok
  TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
  TFT.setTextSize(3);
  TFT.setTextColor(ILI9341_GREEN);
  TFT.println("VIDI X kamera     Web Stream");
  TFT.setTextSize(1);
  TFT.setTextColor(ILI9341_RED);
  TFT.println("Monitor prikazuje analogni PIN 14 - Mkrofon");
  Serial.println("Monitor prikazuje analogni PIN 14 - Mkrofon");

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 8000000;
  //config.pixel_format = PIXFORMAT_JPEG; // NE RADI
  //config.pixel_format = PIXFORMAT_YUV422;
  config.pixel_format = PIXFORMAT_RGB565;
  //config.pixel_format = PIXFORMAT_GRAYSCALE;

  // for larger pre-allocated frame buffer.
  if (psramFound()) {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 63;
    config.fb_count = 1;
    Serial.printf("psramFound ");
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
    Serial.printf("psram NOT Found ");
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  Serial.println(s->id.PID);

  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  //s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_WROVER_KIT) || defined(CAMERA_MODEL_VIDI_X) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
  //s->set_brightness(s, 1); // up the brightness just a bit
  //s->set_saturation(s, -2); // lower the saturation
  //s->set_framesize(s, FRAMESIZE_CIF);
#endif

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
  TFT.print("Camera Ready! Use 'http://");
  TFT.print(WiFi.localIP());
  TFT.println("' to connect");

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }
  //capturePhotoSaveSpiffs();
  //capturePhotoSaveSD();

  //Ugasi Wi-Fi kako bi oslobodili ADC2 odnosno konkretnije PIN 14 za detektiranje audia
  //Naravno kada nam bude trebala mreža za video stream server, moramo uplaiti WI-FI
  //UgasiWiFi();
}

void UgasiWiFi() {
  Serial.print ("POSLAN ");
  Serial.println ( POSLAN );
  Serial.print ("Timer ");
  Serial.println ( Timer );
  if (POSLAN != 1 && Timer > 100 )
  {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Timer = 0;
    Serial.print ( "Timer = 0;  - Pogledaj vrijeme" );
    pinMode(analogInPin1, OUTPUT);   // Postavi pin
    delay(100);
    digitalWrite(analogInPin1, LOW);   // Postavi pin
    delay(100);
    pinMode(analogInPin1, INPUT);   // Postavi pin
  }
  if (POSLAN == 1 && Timer > 5000 )
  {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Timer = 0;
    POSLAN = 0;
    Serial.print ( "Timer = 0;  - Pogledaj vrijeme" );
  }
}

// Funkcija za detekciju nekog zvuka
boolean zvuk() {
  j = -1;
  int valMin = 4096;     // ove varijeble postavljemo na besmisleno velike vrijednosti
  int valMax = -4096;    // ..koje nam softver neće javiti
  dacWrite(25, 0); //Potrebno je utišati zvučnik kada ga ne koristimo
  while ( j++ <= myWidth ) //Pokreni petlju koja se vrti u trajanju širine ekrana
  {
    int val = analogRead(analogInPin1);    // Očitaj stanje mikrofona
    //TFT.fillRect(j, 50, 4, myHeight, ILI9341_BLACK); // Iscrtaj crni pravokutnik
    TFT.fillRect(j, 110, 5, 40, ILI9341_BLACK); // Iscrtaj crni pravokutnik
    TFT.drawPixel(j, map(val, 50, 4095, myHeight, 1), ILI9341_YELLOW); // grafički prikaži vrijednost mikrofona

    if (valMin > val) { // Ukoliko je dosegnut novi minimum
      valMin = val;     // postavi DeltaMin vrijednost
    }
    if (valMax < val) {  // Ukoliko je dosegnut novi maksimum
      valMax = val;      // postavi DeltaMax vrijednost
    }
  }

  if (valMin < (2000 - prag) && valMax > (2000 + prag)) // Prelaze li vrijednosti ove uvijete
  {
    Play();
    WiFiConn();   // Pokeni funkciju Spajanja Mreže
    sendMessage();// Ovdje šaljemo mail
    return 0;
  }
  else {
    return 1;
  }
}

void WiFiConn() {
  // Spajamo se na Wi-Fi uz pomoć ranije definiranog passworda
  WiFi.begin(ssid, password);
  TFT.fillScreen(ILI9341_BLACK); // obojaj zaslon u crno
  TFT.setCursor(0, 0);
  TFT.setTextSize(3);
  TFT.setTextColor(ILI9341_GREEN);
  TFT.println("VIDI X kamera     Web Stream");
  Serial.print ( "Ponovno spajanje" );
  TFT.setTextSize(1);
  TFT.setTextColor(ILI9341_RED);
  TFT.print("Ponovno spajanje ");

  // Pričekaj dok se konekcija ne ostvari
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
    TFT.print(".");
  }
  TFT.setTextColor(ILI9341_YELLOW);
  TFT.println("- spojen sam!");
  TFT.print("Camera Ready! Use 'http://");
  TFT.print(WiFi.localIP());
  TFT.println("' to connect");
}

void loop() {
  zvuk();  // Pozovi funkciju za detekciju zvuka
  Timer++;
  UgasiWiFi();
}

void sendMessage( void ) {
  // Preparing email
  Serial.println("Sending email...");
  TFT.println("Sending email...");
  /** Enable the debug via Serial port
    none debug or 0
    basic debug or 1
    Debug port can be changed via ESP_MAIL_DEFAULT_DEBUG_PORT in ESP_Mail_FS.h
  */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Declare the message class */
  SMTP_Message message;

  /* Enable the chunked data transfer with pipelining for large message if server supported */
  //message.enable.chunking = true;

  /* Set the message headers */
  message.sender.name = "VIDI X Mail";
  message.sender.email = AUTHOR_EMAIL;

  message.subject = "Detektiran je zvuk";
  message.addRecipient("user1", "mail@domena.hr");

  String htmlMsg = "<span style=\"color:#ff0000;\">Spojite se na server i pogledajte video stream kako bi otkrili što se dogodilo.<br/>Stream se GASI za 10 minuta!</span><br/>";
  htmlMsg = htmlMsg + "<p>Kamera je spremna! Lokalni IP za spajanje je <a href=\"http://";
  htmlMsg = htmlMsg + WiFi.localIP().toString().c_str(); // Konvertiranje IP adrese u string
  htmlMsg = htmlMsg + "\" target=\"_blank\">'http://";
  htmlMsg = htmlMsg + WiFi.localIP().toString().c_str(); // Konvertiranje IP adrese u string
  htmlMsg = htmlMsg + "'</a></p> <br/>";
  message.html.content = htmlMsg.c_str();
  /** The content transfer encoding e.g.
     enc_7bit or "7bit" (not encoded)
     enc_qp or "quoted-printable" (encoded) <- not supported for message from blob and file
     enc_base64 or "base64" (encoded)
     enc_binary or "binary" (not encoded)
     enc_8bit or "8bit" (not encoded)
     The default value is "7bit"
  */
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_base64;

  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_normal;
  message.addHeader("Message-ID: <user1@gmail.com>");

  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;

  /* Start sending the Email and close the session */
  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Error sending Email, " + smtp.errorReason());
    TFT.print("Error sending Email, " + smtp.errorReason());
    POSLAN = 0;
  }
  //to clear sending result log
  smtp.sendingResult.clear();

  ESP_MAIL_PRINTF("Free Heap: %d\n", MailClient.getFreeHeap());
}

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status)
{
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success())
  {
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;
    TFT.print("E-mail je uspjesno poslan");
    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");

    //You need to clear sending result as the memory usage will grow up as it keeps the status, timstamp and
    //pointer to const char of recipients and subject that user assigned to the SMTP_Message object.
    //Because of pointer to const char that stores instead of dynamic string, the subject and recipients value can be
    //a garbage string (pointer points to undefind location) as SMTP_Message was declared as local variable or the value changed.
    smtp.sendingResult.clear();
    POSLAN = 1;
  }
}
