#if defined(CAMERA_MODEL_VIDI_X)
//#if defined(CAMERA_MODEL_WROVER_KIT)
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    15 //- D/C LCD - XLK //bio je na 21
#define SIOD_GPIO_NUM    33 // I2C_SDA
#define SIOC_GPIO_NUM    32 // I2C_SCL 
// adc2_pins 0, 2, 4, 12, 13, 14, 15, 25, 26, 27, 
#define Y9_GPIO_NUM      35 // BTN_1
#define Y8_GPIO_NUM      34 // BTN_2
#define Y7_GPIO_NUM      39 // START            - D5
#define Y6_GPIO_NUM      36 // ADC_BAT          - D4
#define Y5_GPIO_NUM      12 //- VSPI_MISO   //bio na 19
#define Y4_GPIO_NUM      13 //- VSPI_SCK    //bio na 18
#define Y3_GPIO_NUM       0 //- VSPI_CS0 //5
#define Y2_GPIO_NUM       4 //- VSPI_CS2
#define VSYNC_GPIO_NUM   27 //- SELECT           - VS
#define HREF_GPIO_NUM    2  //- VSPI_MOSI        - HS  //bio je na 23
#define PCLK_GPIO_NUM    22 // VSPI_CS1 SD CARD - PLK

#elif defined(CAMERA_MODEL_WROVER_KIT_old)
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    21
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27

#define Y9_GPIO_NUM      35 //I
#define Y8_GPIO_NUM      34 //I
#define Y7_GPIO_NUM      39 //I
#define Y6_GPIO_NUM      36 //I
#define Y5_GPIO_NUM      19
#define Y4_GPIO_NUM      18
#define Y3_GPIO_NUM       5
#define Y2_GPIO_NUM       4
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

#elif defined(CAMERA_MODEL_ESP_EYE)
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    4
#define SIOD_GPIO_NUM    18
#define SIOC_GPIO_NUM    23

#define Y9_GPIO_NUM      36
#define Y8_GPIO_NUM      37
#define Y7_GPIO_NUM      38
#define Y6_GPIO_NUM      39
#define Y5_GPIO_NUM      35
#define Y4_GPIO_NUM      14
#define Y3_GPIO_NUM      13
#define Y2_GPIO_NUM      34
#define VSYNC_GPIO_NUM   5
#define HREF_GPIO_NUM    27
#define PCLK_GPIO_NUM    25

#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM    21 // D/C LCD
#define SIOD_GPIO_NUM    33 // I2C_SDA
#define SIOC_GPIO_NUM    32 // I2C_SCL 

#define Y9_GPIO_NUM      35 // BTN_1
#define Y8_GPIO_NUM      34 // BTN_2
#define Y7_GPIO_NUM      39 // START
#define Y6_GPIO_NUM      36 // ADC_BAT
#define Y5_GPIO_NUM      19 // VSPI_MISO
#define Y4_GPIO_NUM      18 // VSPI_SCK
#define Y3_GPIO_NUM       5 // VSPI_CS0
#define Y2_GPIO_NUM       4 // VSPI_CS2
#define VSYNC_GPIO_NUM   27 // SELECT
#define HREF_GPIO_NUM    23 // VSPI_MOSI
#define PCLK_GPIO_NUM    22 // VSPI_CS1 SD CARD

#elif defined(CAMERA_MODEL_M5STACK_V2_PSRAM)
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM     27
#define SIOD_GPIO_NUM     22
#define SIOC_GPIO_NUM     23

#define Y9_GPIO_NUM       19
#define Y8_GPIO_NUM       36
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM        5
#define Y4_GPIO_NUM       34
#define Y3_GPIO_NUM       35
#define Y2_GPIO_NUM       32
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     26
#define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_M5STACK_WIDE)
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM     27
#define SIOD_GPIO_NUM     22
#define SIOC_GPIO_NUM     23

#define Y9_GPIO_NUM       19
#define Y8_GPIO_NUM       36
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM        5
#define Y4_GPIO_NUM       34
#define Y3_GPIO_NUM       35
#define Y2_GPIO_NUM       32
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     26
#define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_M5STACK_ESP32CAM)
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM     27
#define SIOD_GPIO_NUM     25
#define SIOC_GPIO_NUM     23

#define Y9_GPIO_NUM       19
#define Y8_GPIO_NUM       36
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM        5
#define Y4_GPIO_NUM       34
#define Y3_GPIO_NUM       35
#define Y2_GPIO_NUM       17
#define VSYNC_GPIO_NUM    22
#define HREF_GPIO_NUM     26
#define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#elif defined(CAMERA_MODEL_TTGO_T_JOURNAL)
#define PWDN_GPIO_NUM      -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM    21 // D/C LCD
#define SIOD_GPIO_NUM    33 // I2C_SDA
#define SIOC_GPIO_NUM    32 // I2C_SCL 

#define Y9_GPIO_NUM      35 // BTN_1
#define Y8_GPIO_NUM      34 // BTN_2
#define Y7_GPIO_NUM      39 // START
#define Y6_GPIO_NUM      36 // ADC_BAT
#define Y5_GPIO_NUM      19 // VSPI_MISO
#define Y4_GPIO_NUM      18 // VSPI_SCK
#define Y3_GPIO_NUM       5 // VSPI_CS0
#define Y2_GPIO_NUM       4 // VSPI_CS2
#define VSYNC_GPIO_NUM   27 // SELECT
#define HREF_GPIO_NUM    23 // VSPI_MOSI
#define PCLK_GPIO_NUM    22 // VSPI_CS1 SD CARD

#else
#error "Camera model not selected"
#endif
