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

/**************************************************************
 * Adafruit Feather M0 basic example
 * Adafruit M4 Grand Central example
 * 
 * Don't forget to add your GPRS credentials (APN, etc).
 * 
 * Uses Serial1 on the Adafruit M0 basic
 * MAKE SURE THE nRF9160DK "SW1" (VDD_IO) (near the power switch) IS SET TO 3V
 * D0 Serial1 RX, connect to nRF9160 UART TX
 * D1 Serial1 TX, connect to nRF9160 UART RX
 * D6 nRF9160 RESET (active low), connect to nRF9160 RESET
 * THERE IS NO RESET PIN (that works) ON THE nRF9160DK, solder a wire onto the RESET button pin
 * Alternativly, press the RESET button as as the Arduino MCU is
 * being reset, timing must be good.
 * 
 * Uses Serial1 on the Adafruit M4 Grand Central
 * MAKE SURE THE nRF9160DK "SW1" (VDD_IO) (near the power switch) IS SET TO 3V
 * D0 Serial1 RX, connect to nRF9160 UART TX
 * D1 Serial1 TX, connect to nRF9160 UART RX
 * D6 nRF9160 RESET (active low), connect to nRF9160 RESET
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

#define NRF9160_RESET_PIN 6

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
//#define DUMP_AT_COMMANDS

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
  SerialAT.begin(115200);
  //SerialAT.begin(921600);
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

#if TINY_GSM_USE_GPRS && defined TINY_GSM_MODEM_XBEE
  // The XBee must run the gprsConnect function BEFORE waiting for network!
  modem.gprsConnect(apn, gprsUser, gprsPass);
#endif

#if TINY_GSM_USE_GPRS && defined TINY_GSM_MODEM_NRF9160
  // The nRG9160 must run the gprsConnect function BEFORE waiting for network to set APN information!
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

#if TINY_GSM_USE_GPRS && !defined TINY_GSM_MODEM_NRF9160
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
