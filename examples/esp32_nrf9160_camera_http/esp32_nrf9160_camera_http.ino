/* The nRF9160 opperates at 3V3, don't use 5 volts with it!
 * 
 * Download the dev package from https://www.drassal.net/filestore/tinygsm_nrf9160_dev_20210202.zip
 *
 * *** DON'T FORGET 1V8 3V3 switch, change it to 3V to work with 3V3 GPIO!!!
 * *** It is default 1V8 when it comes from the factory!!!
 *  
 *  For these examples an ESP32-CAM was used for the reason that it can send JPEG data through the LTE
 *  connection to test speed and reliability.  Other boards used are the Adafruit Feather M0 basic
 *  and the Adafruit M4 Grand Central, others may work, UART configuration needs to be changed.
 * 
 * For this example connect the nRF9160 UART to pins 13 (RX) and 12 (TX) of the ESP32-CAM
 * This can be changed by configuring the Serial object to match your hardware
 * Remove the HardwareSerial define and related HardwareSerial to change to a "normal" serial port
 * 
 * The firmware for the nRF9160 will differ by the board.  For all the firmware the hardware flow control is disabled.
 * It would be good to have hardare flow control, but how do we do it with Atduino???
 * 
 * For the nRF9160DK the UART pins are P0.10 (TX) and P0.11 (RX), 115200 or 921600 depending on the HEX file loaded.
 * Besides these pins RESET needs to be connected, in this example RESET is pin 15 of the ESP32-CAM
 * This RESET pin needs to be soldered to soldered to the nRF9160DK, or you need to press the
 * RESET button as TinyGSM attempts to reset the nRF9160.  The RESET header pin does not appear
 * to be connected to the RESET signal of the nRF9160 on the nRF9160DK.
 * Power the nRF9160DK from the USB cable and connect the two ground pins together on ESP32-CAM and nRF9160DK.
 * Also, remember to set the GPIO voltage switch on the nRF9160DK to 3 (actually 3V3), this is SW1 (VDD IO).
 * 
 * If using the Feather nRF9160 https://www.jaredwolff.com/store/nrf9160-feather/
 * Then connections to the UART will be on P0.01 (RX) (marked D5 on the PCB) and P0.03 (TX) (marked D7 on the PCB)
 * This was done as an attempt to align the UART pins with the hardware UART pins on the Adafruit Feather M0 basic.
 * The UART speed is 115200 or 921600 depending on the HEX file loaded.  In addition the RESET pin needs to be
 * connected, this is the pin marked "RST" on the Feather nRF9160.
 * 
 * If using the Actinius Icarus IoT Board
 * https://www.actinius.com/icarus
 * https://www.adafruit.com/product/4753
 * Be aware this board does not come with an LTE antenna, you need to order one for it!
 * This board's UART is configured the same as the Feather nRF9160
 * The UART pins are P0.01 (RX) (marked 1 on the PCB), and P0.03 (TX) (marked 3 on the PCB).
 * The reset pin marked "rst" also needs to be connected to the ESP32-CAM.
 * 
 * The following firmware files are included along with source.
 * The nRF Connect development software and the nRF9160 SDK can be downloaded, based off Zephyr.
 * 
 * Updating the modem baseband firmware should also be done if it is not version 1.2.3.
 * 
 * In order to load firmware nrfjprog is required, these below commands have been tested on MacOS but
 * should also work on Linux, Windows should be similar.
 * 
 * The prebuilt firmware can be loaded as follows...
 * For the nRF9160DK the following commands.
 * 
 * 
 * Bring the nRF9160 baseband up to date
 * *** Only have to do this once oer chip ***
 * nRF9160 modem baseband update
 * *** DON'T FORGET 1V8 3V3 switch, change it to 3V to work with 3V3 programming targets!!!
 * *** If programming the nRF9160DK, DON'T FORGET to change the nRF52/nRF91 switch to nRF91 to program the nRF52840
 * python3 nrf9160_baseband/flash_with_logging.py mfw_nrf9160_1.2.3.zip
 * 
 * 
 * *** If programming the nRF9160DK, DON'T FORGET to change the nRF52/nRF91 switch to nRF91 to program the nRF52840
 * *** Reprogram this if you want to change the BAUD rate, or are recompiling and changing the source ***
 * *** DON'T FORGET 1V8 3V3 switch, change it to 3V to work with 3V3 programming targets!!!
 * nrfjprog -f NRF91 --program serial_lte_modem_actinius_icarus_115200_merged.hex --sectorerase
 * nrfjprog -f NRF91 --program serial_lte_modem_actinius_icarus_921600_merged.hex --sectorerase
 * nrfjprog -f NRF91 --program serial_lte_modem_circuitdojo_feather_nrf9160_115200_merged.hex --sectorerase
 * nrfjprog -f NRF91 --program serial_lte_modem_circuitdojo_feather_nrf9160_921600_merged.hex --sectorerase
 * nrfjprog -f NRF91 --program serial_lte_modem_nrf9160dk_115200_merged.hex --sectorerase
 * nrfjprog -f NRF91 --program serial_lte_modem_nrf9160dk_921600_merged.hex --sectorerase
 * 
 * 
 * If using the nRF9160DK, in addition bring the board controller (nRF52840) up to date
 * *** Only have to do this once per nRF9160DK ***
 * nRF9160DK board controller (nRF52840) firmware
 * *** DON'T FORGET to change the nRF52/nRF91 switch to nRF52 to program the nRF52840
 * nrfjprog -f NRF52 --program nRF9160_DK_board_controller_FW.hex --sectorerase
 */
 
#include "esp_camera.h"
#include <TimeLib.h>

#define UPLOAD_HOST        "yourserver.com"
#define UPLOAD_PORT        80
#define UPLOAD_PATH_STORE  "/nrf9160_test/"
#define UPLOAD_PATH_SCRIPT "/nrf9160_test/upload.php"
#define UPLOAD_FILENAME    "picture.jpg"
#define UPLOAD_TOKEN       "asdw"
#define UPLOAD_MESSAGE     "A54S89EF5"

// This project was tested with the AI Thinker Model, M5STACK PSRAM Model and M5STACK WITHOUT PSRAM
#define CAMERA_MODEL_AI_THINKER
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WITHOUT_PSRAM

// Not tested with this model
//#define CAMERA_MODEL_WROVER_KIT

#if defined(CAMERA_MODEL_WROVER_KIT)
  #define PWDN_GPIO_NUM    -1
  #define RESET_GPIO_NUM   -1
  #define XCLK_GPIO_NUM    21
  #define SIOD_GPIO_NUM    26
  #define SIOC_GPIO_NUM    27
  
  #define Y9_GPIO_NUM      35
  #define Y8_GPIO_NUM      34
  #define Y7_GPIO_NUM      39
  #define Y6_GPIO_NUM      36
  #define Y5_GPIO_NUM      19
  #define Y4_GPIO_NUM      18
  #define Y3_GPIO_NUM       5
  #define Y2_GPIO_NUM       4
  #define VSYNC_GPIO_NUM   25
  #define HREF_GPIO_NUM    23
  #define PCLK_GPIO_NUM    22

#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
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
  #define Y2_GPIO_NUM       32
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_M5STACK_WITHOUT_PSRAM)
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

  #define BUTTON0           14 //can't use 16, it is somehow connected to the PSRAM
#else
  #error "Camera model not selected"
#endif

void setup_camera() {
  camera_config_t config;
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
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    Serial.println("psram found");
    config.frame_size = FRAMESIZE_UXGA;  // 1600x1200
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    Serial.println("psram not found");
    config.frame_size = FRAMESIZE_SVGA;  // 800x600
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void checkPsram(void) {
  //All capacity of SPRAM is secured
  int memMax = ESP.getFreePsram();
  char *p = (char*) ps_calloc( memMax , sizeof(char) );
  if (p == NULL) {
    Serial.println("Unable to allocate memory");
  }

  //Check whether value can be written for each byte
  int i = 0;
  while ( i < memMax )
  {
    p[i] = (char)i; //Write value
    if ( p[i] != (char)i )  //Check if the values match
    {
      Serial.printf("write error at %d\n", i);
      i--;
      break;
    }
    i++;
  }

  Serial.printf("%d bytes check Ok\n", i);
  
  free(p); //Freeing memory  
}

void displayMemoryInfo(void) {
  Serial.printf("Total heap  : %d\n", ESP.getHeapSize());
  Serial.printf("Free heap   : %d\n", ESP.getFreeHeap());
  Serial.printf("Total PSRAM : %d\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM  : %d\n", ESP.getFreePsram());  
}

void displayBoardInfo(void) {
  Serial.printf("Internal Total heap %d, internal Free Heap %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
  Serial.printf("SPIRam Total heap %d, SPIRam Free Heap %d\n", ESP.getPsramSize(), ESP.getFreePsram());
  Serial.printf("Flash Size %d, Flash Speed %d\n", ESP.getFlashChipSize(), ESP.getFlashChipSpeed());
  Serial.printf("ChipRevision %d, Cpu Freq %d, SDK Version %s\n", ESP.getChipRevision(), ESP.getCpuFreqMHz(), ESP.getSdkVersion());
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  Serial.println();
  displayBoardInfo();
  Serial.println();
  displayMemoryInfo();
  Serial.println();
  Serial.println("Checking PSRAM");
  checkPsram();
  Serial.println();

  setup_camera();

  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  //s->set_vflip(s, 1);//flip it back
  //s->set_brightness(s, 1);//up the blightness just a bit
  //s->set_saturation(s, -2);//lower the saturation

  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);  // 320x240
  //s->set_framesize(s, FRAMESIZE_SVGA);  // 800x600

  if(!setupModem()) {
    Serial.println("setupModem failed...HALT");
    while(1);
  }

  pinMode(BUTTON0, INPUT_PULLUP);
}

void displayStamp(void) {
  time_t timeStamp = nrf9160_getEpoch();

  char temp[50];
  Serial.print("timeEpoch   :");
  Serial.println((uint32_t)timeStamp);
  sprintf(temp, "%04d/%02d/%02d %02d:%02d:%02d UTC",
                year(timeStamp),
                month(timeStamp),
                day(timeStamp),
                hour(timeStamp),
                minute(timeStamp),
                second(timeStamp));
  Serial.print("timeStamp   :");
  Serial.println(temp);
  timeStamp += (9 * 60 * 60);
  sprintf(temp, "%04d/%02d/%02d %02d:%02d:%02d JST",
                year(timeStamp),
                month(timeStamp),
                day(timeStamp),
                hour(timeStamp),
                minute(timeStamp),
                second(timeStamp));
  Serial.print("             ");
  Serial.println(temp);
}

static esp_err_t stream_handler(void){
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;

  //cycle through one buffer to get current image
  fb = esp_camera_fb_get();
  esp_camera_fb_return(fb);
  fb = NULL;

  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    res = ESP_FAIL;
  } else {
    if(fb->format != PIXFORMAT_JPEG){
      bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
      esp_camera_fb_return(fb);
      fb = NULL;
      if(!jpeg_converted){
        Serial.println("JPEG compression failed");
        res = ESP_FAIL;
      }
    } else {
      _jpg_buf_len = fb->len;
      _jpg_buf = fb->buf;
    }
  }


  // send the image here, it is in _jpeg_buf, with a length of _jpg_buf_len
  Serial.print("Uploading image to ");
  Serial.print(UPLOAD_HOST);
  Serial.print(":");
  Serial.print(UPLOAD_PORT);
  Serial.println(UPLOAD_PATH_SCRIPT);
  Serial.print("View image at http://");
  Serial.print(UPLOAD_HOST);
  Serial.print(UPLOAD_PATH_STORE);
  Serial.println(UPLOAD_FILENAME);
  displayStamp();
  sendImage(UPLOAD_TOKEN, UPLOAD_MESSAGE, _jpg_buf, _jpg_buf_len, UPLOAD_HOST, UPLOAD_PORT, UPLOAD_PATH_SCRIPT, UPLOAD_FILENAME);


  if(fb){
    esp_camera_fb_return(fb);
    fb = NULL;
    _jpg_buf = NULL;
  } else if(_jpg_buf){
    free(_jpg_buf);
    _jpg_buf = NULL;
  }

  return res;
}

void loop() 
{
    if(digitalRead(BUTTON0) == false) {
      stream_handler();
      displayMemoryInfo();
    }
}
