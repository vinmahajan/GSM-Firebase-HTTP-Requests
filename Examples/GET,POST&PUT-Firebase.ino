#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER 256  //Increase RX buffer if get data is more


#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

//////////////////////////////////////////////////   choose the microcontroller  //////////////////////////////////////////////

//////////// for Arduino uno/nano
#include <SoftwareSerial.h>
SoftwareSerial GSM(3, 2);


//////////// for Esp32
// #include <HardwareSerial.h>
// HardwareSerial GSM(1)  //if using UART1      //or     //HardwareSerial GSM(2)  //if using UART2


//////////// for Raspberry pi pico
// #ifdef ARDUINO_ARCH_MBED_RP2040
// UART Serial2(8, 9, NC, NC);
// #else
// #endif

///////////////////////////////////////////////    Firebase credentials  ///////////////////////////////////////////////////

// for gsm firebase
const char FIREBASE_HOST[] = "sterimix-default-rtdb.firebaseio.com";
const String FIREBASE_AUTH = "HyHnShIRtynRDAClEpnjdMOaslC6o3iX4ubJXBvl";
const String GET_PATH = "Example_GET/";
const String POST_PATH = "Example_POST/";
const String PUT_PATH = "Example_PUT/";
const int SSL_PORT = 443;

const char APN[] = "airtelgprs.com";
const char USER[] = "";
const char PASS[] = "";


TinyGsm modem(GSM);
TinyGsmClientSecure client(modem);
HttpClient http(client, FIREBASE_HOST, SSL_PORT);


// Setup
void setup() {
  Serial.begin(9600);
  delay(500);

  GSM.begin(9600);  // if using arduino

  // GSM.begin(9600, SERIAL_8N1, 4, 2);  //  or GSM.begin(9600, SERIAL_8N1, 16, 17); if using esp32

  // Serial2.begin(9600);     // if using pi pico


  delay(500);
  Serial.println("Initializing modem...");
  modem.restart();

  Serial.print(F("Waiting for network..."));
  if (!modem.waitForNetwork()) {
    Serial.println("fail to connect network");
    delay(1000);
    return;
  } else {
    Serial.println("Connected to network");
  }
  modem.gprsConnect(APN, USER, PASS);
  http.connect(FIREBASE_HOST, SSL_PORT);
  ConnectGPRS_HTTP();
}



/////////  main loop
void loop() {

  String Response = GET_Firebase();
  delay(5000);

  String data = "{\"Data1\":" + String(123) + ",\"Data2\":\"data02\"}";
  POST_Firebase(data);
  delay(5000);

  PUT_Firebase(data);
  delay(5000);
}



//////////////////////////////////////////////////////////////////////////////////////
// GSM & Firebase Functions
/////////////////////////////////////////////////////////////////////////////////////
String GET_Firebase() {
  Serial.println();
  Serial.println("Performing HTTPS GET request... ");

  if (!ConnectGPRS_HTTP()) {
    Serial.println("Connecting to GPRS & HTTP...");
    delay(500);
    ConnectGPRS_HTTP();
  }

  http.connectionKeepAlive();  // Currently, this is needed for HTTPS
  String url = "/" + GET_PATH + ".json?auth=" + FIREBASE_AUTH;
  Serial.print("GET:");
  Serial.println(url);
  http.get(url);

  // read the status code and body of the response
  //statusCode-200 (OK) | statusCode -3 (TimeOut)
  int statusCode = http.responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  String response = http.responseBody();
  Serial.print("Response: ");
  Serial.println(response);
  if (statusCode != 200) {
    return "0";
  }
  return response;
}



bool POST_Firebase(String data) {
  Serial.println();
  Serial.println("Performing HTTPS POST request... ");

  if (!ConnectGPRS_HTTP()) {
    Serial.println("Connecting to GPRS & HTTP.");
    delay(500);
    ConnectGPRS_HTTP();
  }

  http.connectionKeepAlive();  // Currently, this is needed for HTTPS
  String url = "/" + POST_PATH + "/.json?auth=" + FIREBASE_AUTH;
  Serial.print("PUT:");
  Serial.println(url);
  Serial.print("Data:");
  Serial.println(data);

  String contentType = "application/json";
  http.post(url, contentType, data);

  // read the status code and body of the response
  //statusCode-200 (OK) | statusCode -3 (TimeOut)
  int statusCode = http.responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  String response = http.responseBody();
  Serial.print("Response: ");
  Serial.println(response);

  if (statusCode == 200) {
    return true;
  }
  return false;
}



bool PUT_Firebase(String data) {
  Serial.println();
  Serial.println("Performing HTTPS POST request... ");

  if (!ConnectGPRS_HTTP()) {
    Serial.println("Connecting to GPRS & HTTP.");
    delay(500);
    ConnectGPRS_HTTP();
  }

  http.connectionKeepAlive();  // Currently, this is needed for HTTPS
  String url = "/" + PUT_PATH + "/.json?auth=" + FIREBASE_AUTH;
  Serial.print("PUT:");
  Serial.println(url);
  Serial.print("Data:");
  Serial.println(data);

  String contentType = "application/json";
  http.put(url, contentType, data);

  // read the status code and body of the response
  //statusCode-200 (OK) | statusCode -3 (TimeOut)
  int statusCode = http.responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);

  if (statusCode == 200) {
    return true;
  }
  return false;
}


bool ConnectGPRS_HTTP() {
  Serial.print("Connecting to ");
  Serial.print(APN);
  // modem.gprsDisconnect();
  // http.stop();
  // modem.gprsConnect(APN, USER, PASS);
  // http.connect(FIREBASE_HOST, SSL_PORT);

  if (!modem.isGprsConnected()) {
    Serial.print(" -> Connecting to GPRS.");
    modem.gprsDisconnect();
    modem.gprsConnect(APN, USER, PASS);
  }
  if (!http.connected()) {
    Serial.print(" -> Connecting to HTTP.");
    http.stop();
    http.connect(FIREBASE_HOST, SSL_PORT);
  }
  if (http.connected() && modem.isGprsConnected()) {
    Serial.println(" -> GPRS & HTTP Connected.");
    return true;
  } else {
    Serial.println(" -> GPRS & HTTP NOT Connected.");
    return false;
  }
  return false;
}


