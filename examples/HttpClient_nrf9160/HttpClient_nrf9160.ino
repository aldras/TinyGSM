/**************************************************************
 *
 * This sketch connects to a website and downloads a page.
 * It can be used to perform HTTP/RESTful API calls.
 *
 * For this example, you need to install ArduinoHttpClient library:
 *   https://github.com/arduino-libraries/ArduinoHttpClient
 *   or from http://librarymanager/all#ArduinoHttpClient
 *
 * TinyGSM Getting Started guide:
 *   https://tiny.cc/tinygsm-readme
 *
 * For more HTTP API examples, see ArduinoHttpClient library
 *
 * NOTE: This example may NOT work with the XBee because the
 * HttpClient library does not empty to serial buffer fast enough
 * and the buffer overflow causes the HttpClient library to stall.
 * Boards with faster processors may work, 8MHz boards will not.
 **************************************************************/

/* The nRF9160 opperates at 3V3, don't use 5 volts with it!
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
 
#include <HardwareSerial.h>
HardwareSerial mySerial(1);

#define NRF9160_RESET_PIN 15

// Select your modem:
//#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM808
// #define TINY_GSM_MODEM_SIM868
// #define TINY_GSM_MODEM_SIM900
// #define TINY_GSM_MODEM_SIM7000
// #define TINY_GSM_MODEM_SIM5360
// #define TINY_GSM_MODEM_SIM7600
// #define TINY_GSM_MODEM_UBLOX
// #define TINY_GSM_MODEM_SARAR4
// #define TINY_GSM_MODEM_M95
// #define TINY_GSM_MODEM_BG96
// #define TINY_GSM_MODEM_A6
// #define TINY_GSM_MODEM_A7
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_MC60
// #define TINY_GSM_MODEM_MC60E
// #define TINY_GSM_MODEM_ESP8266
// #define TINY_GSM_MODEM_XBEE
// #define TINY_GSM_MODEM_SEQUANS_MONARCH
#define TINY_GSM_MODEM_NRF9160

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1

// or Software Serial on Uno, Nano
//#include <SoftwareSerial.h>
//SoftwareSerial SerialAT(2, 3); // RX, TX

// Increase RX buffer to capture the entire response
// Chips without internal buffering (A6/A7, ESP8266, M590)
// need enough space in the buffer for the entire response
// else data will be lost (and the http library will fail).
#if !defined(TINY_GSM_RX_BUFFER)
#define TINY_GSM_RX_BUFFER 650
#endif

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon
// #define LOGGING  // <- Logging is for the HTTP library

// Range to attempt to autobaud
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

// Add a reception delay - may be needed for a fast processor at a slow baud rate
// #define TINY_GSM_YIELD() { delay(2); }

// Define how you're planning to connect to the internet
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[]  = "";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Your WiFi connection credentials, if applicable
const char wifiSSID[]  = "YourSSID";
const char wifiPass[] = "YourWiFiPass";

// Server details
const char server[] = "vsh.pp.ua";
const char resource[] = "/TinyGSM/logo.txt";
const int  port = 80;

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

// Just in case someone defined the wrong thing..
#if TINY_GSM_USE_GPRS && not defined TINY_GSM_MODEM_HAS_GPRS
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI true
#endif
#if TINY_GSM_USE_WIFI && not defined TINY_GSM_MODEM_HAS_WIFI
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#endif

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

TinyGsmClient client(modem);
HttpClient http(client, server, port);

void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // !!!!!!!!!!!
  // Set your reset, enable, power pins here
  // !!!!!!!!!!!
  pinMode(NRF9160_RESET_PIN, OUTPUT);
  digitalWrite(NRF9160_RESET_PIN, LOW);
  delay(1000);

  SerialMon.println("Resetting modem (6s)...");

  // Set GSM module baud rate
  //TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  SerialAT.begin(115200, SERIAL_8N1, 13, 12); //<- set your RX/TX Pin
  digitalWrite(NRF9160_RESET_PIN, HIGH);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();
  // modem.init();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

#if TINY_GSM_USE_GPRS
  // Unlock your SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    modem.simUnlock(GSM_PIN);
  }
#endif
}

void loop() {

#if TINY_GSM_USE_WIFI
  // Wifi connection parameters must be set before waiting for the network
  SerialMon.print(F("Setting SSID/password..."));
  if (!modem.networkConnect(wifiSSID, wifiPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");
#endif

#if TINY_GSM_USE_GPRS && (defined TINY_GSM_MODEM_XBEE || defined TINY_GSM_MODEM_NRF9160)
  // The XBee must run the gprsConnect function BEFORE waiting for network!
  modem.gprsConnect(apn, gprsUser, gprsPass);
#endif

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

#if TINY_GSM_USE_GPRS
  // GPRS connection parameters are usually set after network registration
    SerialMon.print(F("Connecting to "));
    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
      SerialMon.println(" fail");
      delay(10000);
      return;
    }
    SerialMon.println(" success");

    if (modem.isGprsConnected()) {
      SerialMon.println("GPRS connected");
    }
#endif

  IPAddress local = IPAddress(0, 0, 0, 0);
  while(local == IPAddress(0, 0, 0, 0)) {
    local = modem.localIP();
    SerialMon.println("Waiting for IP address...");
    delay(500);
  }
  SerialMon.print("Local IP: ");
  SerialMon.println(local);

  SerialMon.print(F("Performing HTTP GET request... "));
  int err = http.get(resource);
  if (err != 0) {
    SerialMon.println(F("failed to connect"));
    delay(10000);
    return;
  }

  int status = http.responseStatusCode();
  SerialMon.print(F("Response status code: "));
  SerialMon.println(status);
  if (!status) {
    delay(10000);
    return;
  }

  SerialMon.println(F("Response Headers:"));
  while (http.headerAvailable()) {
    String headerName = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    SerialMon.println("    " + headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0) {
    SerialMon.print(F("Content length is: "));
    SerialMon.println(length);
  }
  if (http.isResponseChunked()) {
    SerialMon.println(F("The response is chunked"));
  }

  String body = http.responseBody();
  SerialMon.println(F("Response:"));
  SerialMon.println(body);

  SerialMon.print(F("Body length is: "));
  SerialMon.println(body.length());

  // Shutdown

  http.stop();
  SerialMon.println(F("Server disconnected"));

#if TINY_GSM_USE_WIFI
    modem.networkDisconnect();
    SerialMon.println(F("WiFi disconnected"));
#endif
#if TINY_GSM_USE_GPRS
    modem.gprsDisconnect();
    SerialMon.println(F("GPRS disconnected"));
#endif

  // Do nothing forevermore
  while (true) {
    delay(1000);
  }
}
