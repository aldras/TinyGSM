/**************************************************************
 * ESP32-CAM example
 * 
 * Don't forget to add your GPRS credentials (APN, etc).
 * 
 * Uses HardwareSerial on the ESP32-CAM
 * MAKE SURE THE nRF9160DK "SW1" (VDD_IO) (near the power switch) IS SET TO 3V
 * IO12 HardwareSerial TX, connect to nRF9160 UART RX
 * IO13 HardwareSerial RX, connect to nRF9160 UART TX
 * IO14 Picture take button, short this to GND to automatically take pictures
 * IO15 nRF9160 RESET (active low), connect to nRF9160 RESET
 * THERE IS NO RESET PIN (that works) ON THE nRF9160DK, solder a wire onto the RESET button pin
 *   Alternativly, press the RESET button as as the Arduino MCU is
 *   being reset, timing must be good.
 * 
 * Also update the nRF51 board controller chip (only needed one time)
 * This has not been from Nordic's distribution found here
 * https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF9160-DK/Download#infotabs
 * MAKE SURE THE "PROG_DEBUG" SWITCH IS SET TO "nrf52" 
 * nrfjprog -f NRF52 --program nRF9160_DK_board_controller_FW.hex
 * 
 * Firmware with _merged.hex is for nRF Connect Programmer, or nrfjprog
 * nrfjprog example shown below.
 * nrfjprog -f NRF91 --program serial_lte_modem_nrf9160dk_115200_merged.hex --sectorerase
 * 
 * The UART pinout on the nRF9160 varies by firmware
 * nrf9160dk (nRF9160DK board)
 *   https://www.nordicsemi.com/Software-and-tools/Development-Kits/nRF9160-DK
 *   MAKE SURE THE "PROG_DEBUG" SWITCH IS SET TO "nrf91" 
 *   firmware file: serial_lte_modem_nrf9160dk_115200_app_signed.hex (for nRF Connect Programmer)
 *   firmware file: serial_lte_modem_nrf9160dk_115200_merged.hex (for nrfjprog)
 *   10 - UART TX
 *   11 - UART RX
 *        hardware flow control disabled
 *   THERE IS NO RESET PIN (that works) ON THE nRF9160DK, solder a wire onto the RESET button pin
 *     Alternativly, press the RESET button as as the Arduino MCU is
 *     being reset, timing must be good.
 *     
 * actinius_icarus (attempted to match Adafruit Feather layout for M0 SERCOM3)
 *   https://www.actinius.com/icarus
 *   https://www.adafruit.com/product/4753
 *   MAKE SURE THE "PROG_DEBUG" SWITCH IS SET TO "nrf91" 
 *   firmware file: serial_lte_modem_actinius_icarus_115200_app_signed.hex (for nRF Connect Programmer)
 *   firmware file: serial_lte_modem_actinius_icarus_115200_merged.hex (for nrfjprog)
 *   1   - UART RX
 *   3   - UART TX
 *         hardware flow control disabled
 *   RST - RESET pin
 *   
 * circuitdojo_feather_nrf9160 (attempted to match Adafruit Feather layout for M0 SERCOM3)
 *   https://www.jaredwolff.com/store/nrf9160-feather/
 *   MAKE SURE THE "PROG_DEBUG" SWITCH IS SET TO "nrf91" 
 *   firmware file: serial_lte_modem_circuitdojo_feather_nrf9160_115200_app_signed.hex (for nRF Connect Programmer)
 *   firmware file: serial_lte_modem_circuitdojo_feather_nrf9160_115200_merged.hex (for nrfjprog)
 *   1   - UART RX (marked D5 on the PCB)
 *   3   - UART RX (marked D7 on the PCB)
 *         hardware flow control disabled
 *   RST - RESET pin
 *   
 * The nRF9160 is flexible in that most pins can be routed to
 * the UART, will require rebuilding the uRF9160 firmware.
 * 
 * If using the nRF9160DK the RESET pin is not broken out
 * and the pin labeled RESET does not appear to reset the
 * nRF9160.  This is bad, I know, but solder a wire on the RESET
 * button pin and connect it to pin15 on the M4 Grand Central.
 * 
 * The prebuilt nRF9160 firmware is available at the following
 * address.
 * 
 * https://www.drassal.net/filestore/esp32_nrf9160_camera_http_20210203/nrf9160_baseband_20210203.zip
 * 
 * Use the nRF Connect Programmer available here.
 * 
 * https://www.nordicsemi.com/Software-and-tools/Development-Tools/nRF-Connect-for-desktop
 * 
 * First install nRF Connect Desktop, then install the "Programmer"
 * from the nRF Connect application.
 * 
 * FIRMWARE DESCIPTION
 * There are multiple firmware files, breakdown of the filename
 * serial_lte_modem_nrf9160dk_115200_app_signed.hex
 *   "serial_lte_modem" is the project nRF9160 SDK's name
 *   "nrf9160dk" is board the firmware was built for
 *   "115200" is the UART speed
 *   "app_signed" is built for loading through a bootloader
 * 
 * serial_lte_modem_nrf9160dk_115200_app_update.bin
 *   "serial_lte_modem" is the project nRF9160 SDK's name
 *   "nrf9160dk" is board the firmware was built for
 *   "115200" is the UART speed
 *   "app_update" is built for loading through a bootloader
 *   
 * serial_lte_modem_nrf9160dk_115200_merged.hex
 *   "serial_lte_modem" is the project nRF9160 SDK's name
 *   "nrf9160dk" is board the firmware was built for
 *   "115200" is the UART speed
 *   "merged" is built for loading a programmer (JLink, JTAG, etc)
 *   
 * The most basic method, if using the nRF9160DK, is to use the
 * nRF Connect programmer with serial_lte_modem_nrf9160dk_115200_app_signed.hex
 * 
 * Be aware this will wipe out any bootloader (if present).
 * 
 * Additionally the following command can be used to program
 * the firmware on the command line.
 * nrfjprog -f NRF91 --program serial_lte_modem_nrf9160dk_115200_merged.hex --sectorerase
 * 
 * If the firmware is working correctly you should get a "Ready"
 * through the UART a few seconds after reset.
 * 
 * The nRF9160 has an ARM Cortex-M33 main MCU and an LTE modem.
 * The LTE modem runs its own firmware and this occasionally
 * needs to be updated.
 * 
 * After loading the main MCU firmware the following AT command
 * can be used to display the current baseband version.
 * AT+CGMR
 * 
 * The modem baseband (nRF9160 SiP modem firmware) (version 1.2.3)
 * used for the above firmware is available at the following address
 * or directly from Nordic as well.
 * 
 * https://www.drassal.net/filestore/esp32_nrf9160_camera_http_20210203/nrf9160_firmware_20210203.zip
 * https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF9160-DK/Download#infotabs
 * 
 * Updating the modem baseband is a task in itself, but the files
 * some directions are included in the above file.
 * 
 * The nRF Connect has a built in baseband updater, it should
 * allow easy updates to the modem baseband using the button
 * "Update modem" button in the lower right corner.
 * 
 * The other method involves using a python3 script available,
 * in the above link, to perform a manual baseband update.
 * 
 **************************************************************/

#include "esp_camera.h"
#include <TimeLib.h>

#define UPLOAD_HOST        "www.yourServer.net"
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
