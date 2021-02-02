#define UART_NRF91_TX     12
#define UART_NRF91_RX     13

#include <HardwareSerial.h>
HardwareSerial mySerial(1);

#define TINY_GSM_MODEM_NRF9160
// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT mySerial
// Increase RX buffer to capture the entire response
// Chips without internal buffering (A6/A7, ESP8266, M590)
// need enough space in the buffer for the entire response
// else data will be lost (and the http library will fail).
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 1024
#endif

// See all AT commands, if wanted
//#define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define NRF9160_RESET_PIN 15
#define TINY_GSM_DEBUG SerialMon

// Range to attempt to autobaud
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

// Uncomment this if you want to use SSL
// #define USE_SSL

#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
// SONY (au) SIM
const char apn[]  = "";
const char gprsUser[] = "";
const char gprsPass[] = "";

/*
// biglobe (docomo) SIM
const char apn[]  = "biglobe.jp";
const char gprsUser[] = "user";
const char gprsPass[] = "0000";
*/

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

#ifdef USE_SSL && defined TINY_GSM_MODEM_HAS_SSL
  TinyGsmClientSecure client(modem);
  int port = 443;
#else
  TinyGsmClient client(modem);
  int port = 80;
#endif

bool setupModem(void) {
  pinMode(NRF9160_RESET_PIN, OUTPUT);
  digitalWrite(NRF9160_RESET_PIN, LOW);
  delay(1000);
  
  // !!!!!!!!!!!
  // Set your reset, enable, power pins here
  // !!!!!!!!!!!

  SerialMon.println("Resetting modem (6s)...");

  // Set GSM module baud rate
  //TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  //SerialAT.begin(BAUD, CONFIG, RX_PIN, TX_PIN);
  SerialAT.begin(115200, SERIAL_8N1, UART_NRF91_RX, UART_NRF91_TX); //<- set your RX/TX Pin
  //SerialAT.begin(921600, SERIAL_8N1, UART_NRF91_RX, UART_NRF91_TX); //<- set your RX/TX Pin
  //SerialAT.begin(1843200, SERIAL_8N1, UART_NRF91_RX, UART_NRF91_TX); //too fast, does not work, nRF9160 problem?
  digitalWrite(NRF9160_RESET_PIN, HIGH);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.print("Initializing modem...");
  if (!modem.restart()) {
  // if (!modem.init()) {
    SerialMon.println(F(" [fail]"));
    SerialMon.println(F("************************"));
    SerialMon.println(F(" Is your modem connected properly?"));
    SerialMon.println(F(" Is your serial speed (baud rate) correct?"));
    SerialMon.println(F(" Is your modem powered on?"));
    SerialMon.println(F(" Do you use a good, stable power source?"));
    SerialMon.println(F(" Try useing File -> Examples -> TinyGSM -> tools -> AT_Debug to find correct configuration"));
    SerialMon.println(F("************************"));
    delay(10000);
    return false;
  }
  SerialMon.println(F(" [OK]"));

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

#if TINY_GSM_USE_GPRS
  // Unlock your SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    modem.simUnlock(GSM_PIN);
  }
#endif

#if TINY_GSM_USE_WIFI
  // Wifi connection parameters must be set before waiting for the network
  lcdMsg("Setting SSID/password...");
  SerialMon.print(F("Setting SSID/password..."));
  if (!modem.networkConnect(wifiSSID, wifiPass)) {
    lcdMsgAppend("NG");
    SerialMon.println(" fail");
    delay(10000);
    return false;
  }
  SerialMon.println(" success");
#endif

#if TINY_GSM_USE_GPRS && defined TINY_GSM_MODEM_NRF9160
  // The nRF9160 must run the gprsConnect function BEFORE waiting for network!
  modem.gprsConnect(apn, gprsUser, gprsPass);
#endif

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork(600000L)) {  // You may need lengthen this in poor service areas
    SerialMon.println(F(" [fail]"));
    SerialMon.println(F("************************"));
    SerialMon.println(F(" Is your sim card locked?"));
    SerialMon.println(F(" Do you have a good signal?"));
    SerialMon.println(F(" Is antenna attached?"));
    SerialMon.println(F(" Does the SIM card work with your phone?"));
    SerialMon.println(F("************************"));
    delay(10000);
    return false;
  }
  SerialMon.println(F(" [OK]"));

#if TINY_GSM_USE_GPRS
  // GPRS connection parameters are usually set after network registration
  SerialMon.print("Connecting to APN ");
  SerialMon.print(apn);
  SerialMon.print("...");
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(F(" [fail]"));
    SerialMon.println(F("************************"));
    SerialMon.println(F(" Is GPRS enabled by network provider?"));
    SerialMon.println(F(" Try checking your card balance."));
    SerialMon.println(F("************************"));
    delay(10000);
    return false;
  }
  SerialMon.println(F(" [OK]"));
#endif

  IPAddress local = IPAddress(0, 0, 0, 0);
  while(local == IPAddress(0, 0, 0, 0)) {
    local = modem.localIP();
    SerialMon.println("Waiting for IP address...");
    delay(500);
  }
  SerialMon.print("Local IP: ");
  SerialMon.println(local);
  return true;
}

#define BOUNDARY          "--------------------------133747188241686651551404"  
#define TIMEOUT           20000

#define HTTP_HEADER_SIZE  500
#define HTTP_BODY_TXT     200
#define HTTP_BODY_DATA    200
#define HTTP_BODY_END     100

bool sendImage(char *token,
               char *message,
               uint8_t *data_pic,
               size_t size_pic,
               char* selectedServer,
               uint16_t selectedPort,
               char* selectedResource,
               char* selectedFilename)
{
  char temp[10];

  Serial.print("size_pic = ");
  Serial.println(size_pic);



  /***** BEGIN BODY TEXT ****/
  uint8_t bodyTxt[HTTP_BODY_TXT];
  size_t bodyTxtLength = 0;

  memcpy(&bodyTxt[bodyTxtLength], "--", strlen("--"));
  bodyTxtLength += strlen("--");

  memcpy(&bodyTxt[bodyTxtLength], BOUNDARY, strlen(BOUNDARY));
  bodyTxtLength += strlen(BOUNDARY);

  memcpy(&bodyTxt[bodyTxtLength], "\r\n", strlen("\r\n"));
  bodyTxtLength += strlen("\r\n");

  memcpy(&bodyTxt[bodyTxtLength], "Content-Disposition: form-data; ", strlen("Content-Disposition: form-data; "));
  bodyTxtLength += strlen("Content-Disposition: form-data; ");

  memcpy(&bodyTxt[bodyTxtLength], "name=\"message\"\r\n\r\n", strlen("name=\"message\"\r\n\r\n"));
  bodyTxtLength += strlen("name=\"message\"\r\n\r\n");

  memcpy(&bodyTxt[bodyTxtLength], message, strlen(message));
  bodyTxtLength += strlen(message);
  
  memcpy(&bodyTxt[bodyTxtLength], "\r\n", strlen("\r\n"));
  bodyTxtLength += strlen("\r\n");

  bodyTxt[bodyTxtLength] = 0;
  /***** END BODY TEXT ****/



  /***** BEGIN BODY DATA ****/
  uint8_t bodyData[HTTP_BODY_DATA];
  size_t bodyDataLength = 0;

  memcpy(&bodyData[bodyDataLength], "--", strlen("--"));
  bodyDataLength += strlen("--");

  memcpy(&bodyData[bodyDataLength], BOUNDARY, strlen(BOUNDARY));
  bodyDataLength += strlen(BOUNDARY);

  memcpy(&bodyData[bodyDataLength], "\r\n", strlen("\r\n"));
  bodyDataLength += strlen("\r\n");

  memcpy(&bodyData[bodyDataLength], "Content-Disposition: form-data; ", strlen("Content-Disposition: form-data; "));
  bodyDataLength += strlen("Content-Disposition: form-data; ");

  memcpy(&bodyData[bodyDataLength], "name=\"imageFile\"; ", strlen("name=\"imageFile\"; "));
  bodyDataLength += strlen("name=\"imageFile\"; ");

  memcpy(&bodyData[bodyDataLength], "filename=\"", strlen("filename=\""));
  bodyDataLength += strlen("filename=\"");

  memcpy(&bodyData[bodyDataLength], selectedFilename, strlen(selectedFilename));
  bodyDataLength += strlen(selectedFilename);

  memcpy(&bodyData[bodyDataLength], "\"\r\n", strlen("\"\r\n"));
  bodyDataLength += strlen("\"\r\n");

  memcpy(&bodyData[bodyDataLength], "Content-Type: image/jpeg\r\n\r\n", strlen("Content-Type: image/jpeg\r\n\r\n"));
  bodyDataLength += strlen("Content-Type: image/jpeg\r\n\r\n");

  bodyData[bodyDataLength] = 0;
  /***** END BODY DATA ****/



  /***** BEGIN BODY DATA DATA ****/
  uint8_t *bodyDataData = data_pic;
  size_t bodyDataDataLength = size_pic;
  /***** END BODY DATA DATA ****/



  /***** BEGIN BODY DATA DATA END ****/
  uint8_t bodyDataDataEnd[3];
  size_t bodyDataDataEndLength = 0;

  memcpy(&bodyDataDataEnd[bodyDataDataEndLength], "\r\n", strlen("\r\n"));
  bodyDataDataEndLength += strlen("\r\n");

  bodyDataDataEnd[bodyDataDataEndLength] = 0;
  /***** END BODY DATA DATA END ****/



  /***** BEGIN BODY END ****/
  uint8_t bodyEnd[HTTP_BODY_END];
  size_t bodyEndLength = 0;

  memcpy(&bodyEnd[bodyEndLength], "--", strlen("--"));
  bodyEndLength += strlen("--");

  memcpy(&bodyEnd[bodyEndLength], BOUNDARY, strlen(BOUNDARY));
  bodyEndLength += strlen(BOUNDARY);

  memcpy(&bodyEnd[bodyEndLength], "--\r\n", strlen("--\r\n"));
  bodyEndLength += strlen("--\r\n");

  bodyEnd[bodyEndLength] = 0;
  /***** END BODY END ****/



  size_t bodyLength = bodyTxtLength + bodyDataLength + bodyDataDataLength + bodyDataDataEndLength + bodyEndLength;



  uint8_t header[HTTP_HEADER_SIZE];
  size_t headerLength;

  memcpy(&header[headerLength], "POST ", strlen("POST "));
  headerLength += strlen("POST ");

  memcpy(&header[headerLength], selectedResource, strlen(selectedResource));
  headerLength += strlen(selectedResource);

  memcpy(&header[headerLength], " HTTP/1.1\r\n", strlen(" HTTP/1.1\r\n"));
  headerLength += strlen(" HTTP/1.1\r\n");

  memcpy(&header[headerLength], "cache-control: no-cache\r\n", strlen("cache-control: no-cache\r\n"));
  headerLength += strlen("cache-control: no-cache\r\n");

  memcpy(&header[headerLength], "Content-Type: multipart/form-data; boundary=", strlen("Content-Type: multipart/form-data; boundary="));
  headerLength += strlen("Content-Type: multipart/form-data; boundary=");

  memcpy(&header[headerLength], BOUNDARY, strlen(BOUNDARY));
  headerLength += strlen(BOUNDARY);

  memcpy(&header[headerLength], "\r\n", strlen("\r\n"));
  headerLength += strlen("\r\n");

  memcpy(&header[headerLength], "User-Agent: PostmanRuntime/6.4.1\r\n", strlen("User-Agent: PostmanRuntime/6.4.1\r\n"));
  headerLength += strlen("User-Agent: PostmanRuntime/6.4.1\r\n");

  memcpy(&header[headerLength], "Accept: */*\r\n", strlen("Accept: */*\r\n"));
  headerLength += strlen("Accept: */*\r\n");

  memcpy(&header[headerLength], "Host: ", strlen("Host: "));
  headerLength += strlen("Host: ");

  memcpy(&header[headerLength], selectedServer, strlen(selectedServer));
  headerLength += strlen(selectedServer);

  memcpy(&header[headerLength], "\r\n", strlen("\r\n"));
  headerLength += strlen("\r\n");

  memcpy(&header[headerLength], "accept-encoding: gzip, deflate\r\n", strlen("accept-encoding: gzip, deflate\r\n"));
  headerLength += strlen("accept-encoding: gzip, deflate\r\n");

  memcpy(&header[headerLength], "Connection: keep-alive\r\n", strlen("Connection: keep-alive\r\n"));
  headerLength += strlen("Connection: keep-alive\r\n");

  memcpy(&header[headerLength], "content-length: ", strlen("content-length: "));
  headerLength += strlen("content-length: ");

  sprintf(temp, "%d", bodyLength);
  memcpy(&header[headerLength], temp, strlen(temp));
  headerLength += strlen(temp);

  memcpy(&header[headerLength], "\r\n\r\n", strlen("\r\n\r\n"));
  headerLength += strlen("\r\n\r\n");

  header[headerLength] = 0;


  SerialMon.print(F("Connecting to "));
  SerialMon.print(selectedServer);
  if (!client.connect(selectedServer, selectedPort)) {
    SerialMon.println(F(" [fail]"));
    delay(10000);
    return false;
  }
  SerialMon.println(F(" [OK]"));


  SerialMon.printf("headerLength       = %d\n", headerLength);
  SerialMon.printf("bodyTxtLength      = %d\n", bodyTxtLength);
  SerialMon.printf("bodyDataLength     = %d\n", bodyDataLength);
  SerialMon.printf("bodyDataDataLength = %d\n", bodyDataDataLength);
  SerialMon.printf("bodyEndLength      = %d\n", bodyEndLength);

  /*
  SerialMon.write(header, headerLength);
  SerialMon.write(bodyTxt, bodyTxtLength);
  SerialMon.write(bodyData, bodyDataLength);
  //SerialMon.write(bodyDataData, bodyDataDataLength);
  SerialMon.write(bodyDataDataEnd, bodyDataDataEndLength);
  SerialMon.write(bodyEnd, bodyEndLength);
  */

  client.write(header, headerLength);
  client.write(bodyTxt, bodyTxtLength);
  client.write(bodyData, bodyDataLength);
  
  //client.write(bodyDataData, bodyDataDataLength);
  //const size_t chunk_size = 8192;
  const size_t chunk_size = 0x4000;
  if(bodyDataDataLength > chunk_size) {
    // chunk and send
    size_t chunk_count = bodyDataDataLength / chunk_size;
    size_t i;
    for(i = 0; i < chunk_count; i++) {
      Serial.printf("Sending chunk %d of %d, %d bytes\n", i, chunk_count, chunk_size);
      Serial.printf("Address %d, %d bytes\n", i * chunk_size, chunk_size);
      client.write((const uint8_t *)&bodyDataData[i * chunk_size], chunk_size);
    }
    // remaining bytes = _jpg_buf_len - (i * chunk_size)
    size_t chunk_remainder = bodyDataDataLength - (i * chunk_size);
    Serial.printf("Sending final chunk, %d bytes\n", chunk_remainder);
    Serial.printf("Address %d, %d bytes\n", i * chunk_size, chunk_remainder);
    client.write((const uint8_t *)&bodyDataData[i * chunk_size], (uint16_t)chunk_remainder);
  } else {
    // send in one packet
    client.write((const uint8_t *)bodyDataData, (uint16_t)bodyDataDataLength);
  }
  
  client.write(bodyDataDataEnd, bodyDataDataEndLength);
  client.write(bodyEnd, bodyEndLength);
  
  delay(20);
  uint32_t timeout = millis();
  uint32_t bytesReceived = 0;
  //selectedRamImage.clear();
  if(client.connected() && millis() - timeout < 10000L) {
    while (client.available()) {
      char c = client.read();
      //selectedRamImage.write((uint8_t)c);
      bytesReceived += 1;
      timeout = millis();
      if(bytesReceived % 5000 == 0) {
        SerialMon.print(F("Received: "));
        SerialMon.print(bytesReceived);
        SerialMon.println(F(" bytes"));
        sprintf(temp, "%d", bytesReceived);
      }
    }
    // if we fall through set a timer to wait for more data
    // if the timer expires then we close the connection
    SerialMon.print(F("Received: "));
    SerialMon.print(bytesReceived);
    SerialMon.print(F(" bytes"));
    SerialMon.println(F(" (complete)"));
    sprintf(temp, "%d", bytesReceived);
  
    client.stop();
  }
  SerialMon.println(F("Server disconnected"));

  SerialMon.println();
  SerialMon.println(F("************************"));
  SerialMon.print  (F(" Received: "));
  SerialMon.print(bytesReceived);
  SerialMon.println(F(" bytes"));
  SerialMon.println(F("************************"));
  return true;
}

time_t tmConvert_t(byte hh, byte mm, byte ss, byte DD, byte MM, int YYYY)
{
  tmElements_t tmSet;
  tmSet.Year = YYYY - 1970;
  tmSet.Month = MM;
  tmSet.Day = DD;
  tmSet.Hour = hh;
  tmSet.Minute = mm;
  tmSet.Second = ss;
  return makeTime(tmSet); 
}

unsigned long nrf9160_setEpoch(void) {
  //String gsmTime = modem.getGSMDateTime(DATE_TIME);
  //Serial.print("GSM Time:");
  //Serial.println(gsmTime);
  //String gsmDate = modem.getGSMDateTime(DATE_DATE);
  //Serial.print("GSM Date:");
  //Serial.println(gsmDate);
  String gsmDateTime = modem.getGSMDateTime(DATE_FULL);
  //Serial.print("GSM DateTime:");
  //Serial.println(gsmDateTime);

  uint16_t stampYear  = 2000 + ((gsmDateTime[0] - 0x30) * 10) + (gsmDateTime[1] - 0x30);
  uint8_t stampMonth  = ((gsmDateTime[3] - 0x30) * 10) + (gsmDateTime[4] - 0x30);
  uint8_t stampDay    = ((gsmDateTime[6] - 0x30) * 10) + (gsmDateTime[7] - 0x30);
  uint8_t stampHour   = ((gsmDateTime[9] - 0x30) * 10) + (gsmDateTime[10] - 0x30);
  uint8_t stampMinute = ((gsmDateTime[12] - 0x30) * 10) + (gsmDateTime[13] - 0x30);
  uint8_t stampSecond = ((gsmDateTime[15] - 0x30) * 10) + (gsmDateTime[16] - 0x30);
  uint8_t stampExtra  = ((gsmDateTime[18] - 0x30) * 10) + (gsmDateTime[19] - 0x30);

  // hour, min, sec, day, month, year
  setTime(stampHour, stampMinute, stampSecond, stampDay, stampMonth, stampYear);

  /*
  char temp[50];
  sprintf(temp, "%04d/%02d/%02d %02d:%02d:%02d+%02d", stampYear, stampMonth, stampDay, stampHour, stampMinute, stampSecond, stampExtra);
  Serial.print("now() = ");
  Serial.println(temp);
  */

  // 1553898611
  //Serial.println(now());
  return now();
}

unsigned long nrf9160_getEpoch(void) {
  //String gsmTime = modem.getGSMDateTime(DATE_TIME);
  //Serial.print("GSM Time:");
  //Serial.println(gsmTime);
  //String gsmDate = modem.getGSMDateTime(DATE_DATE);
  //Serial.print("GSM Date:");
  //Serial.println(gsmDate);
  String gsmDateTime = modem.getGSMDateTime(DATE_FULL);
  //Serial.print("GSM DateTime:  ");
  //Serial.println(gsmDateTime);

  uint16_t stampYear  = 2000 + ((gsmDateTime[0] - 0x30) * 10) + (gsmDateTime[1] - 0x30);
  uint8_t stampMonth  = ((gsmDateTime[3] - 0x30) * 10) + (gsmDateTime[4] - 0x30);
  uint8_t stampDay    = ((gsmDateTime[6] - 0x30) * 10) + (gsmDateTime[7] - 0x30);
  uint8_t stampHour   = ((gsmDateTime[9] - 0x30) * 10) + (gsmDateTime[10] - 0x30);
  uint8_t stampMinute = ((gsmDateTime[12] - 0x30) * 10) + (gsmDateTime[13] - 0x30);
  uint8_t stampSecond = ((gsmDateTime[15] - 0x30) * 10) + (gsmDateTime[16] - 0x30);
  uint8_t stampExtra  = ((gsmDateTime[18] - 0x30) * 10) + (gsmDateTime[19] - 0x30);

  // hour, min, sec, day, month, year
  time_t timeStamp = tmConvert_t(stampHour, stampMinute, stampSecond, stampDay, stampMonth, stampYear);

  /*
  char temp[50];
  sprintf(temp, "%04d/%02d/%02d %02d:%02d:%02d+%02d", stampYear, stampMonth, stampDay, stampHour, stampMinute, stampSecond, stampExtra);
  Serial.print("timeDate    :");
  Serial.println(temp);
  Serial.print("timeStamp   :");
  Serial.println((uint32_t)timeStamp);
  */

  return timeStamp;
}
