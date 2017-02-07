#define myName "/ext/file.bin"
#define version "0.3.5"

//add the following line to the build option
//build_flags = '-DSSID_PREP="your_wifi_ssid_goes_here"' '-DPASS_PREP="your_wifi_password_goes_here"' '-DAUTH_PREP="your_token_goes_here"'
//
//pin to connect
//EN 5V
//GPIO0 5V
//GPIO15 0V

#define numL 200//Number of reading for internal adc

#include <Arduino.h>
//DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
//TIMER
#include <SimpleTimer.h>

//ADC
#include <Wire.h>
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads;
//WIFI
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>


/*static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;*/

int ONE_WIRE_BUS = 12;//ds18b20 pin

//int sda = 13;
//int scl = 15;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

SimpleTimer timer;


char ssid[] = SSID_PREP;//wifi ssid
char pass[] = PASS_PREP;//wifi password
char auth[] = AUTH_PREP;//blynk token
const char server_upd[] = "192.168.1.124";//ip of the update server
const char server_blynk[] = "192.168.1.124";//ip of the blynk server
const int port_upd = 8090;//port for searching update
const int port_blynk = 8080;//blynk port
const int port_telegram = 8091;//telegram port

const double Kc = 0.183105487805;//constant for converting adc to real voltage

int sendData(float, String);//function to send float trough http api
int sendData(int, String);//function to send int trough http api
int sendVersion(String);//function to send firmware version trough http api
double readVin();//function to read vin
void sendlowbat();//function to send low bat alert to telegram chat

void spedisciDati();//function to send data to blynk server
void aggiornaDormi();//function to search update and deepSleep


const long deltat = 1000*15;//max time to be awake
const long deltat2 = 1000*2;//delay beetwen send data to server

const long tsleep = 1000*1000*60*5;//time to sleep (5 mins.)

void setup(){

  Serial.begin(115200);
  Serial.setDebugOutput(true);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)  {
    delay(100);
    Serial.println(WiFi.status());
  }

  sensors.begin();//init ads sensor

  timer.setInterval(deltat2, spedisciDati);
  timer.setInterval(deltat, aggiornaDormi);

  //--**ADC
  ads.begin(D2, D1);//init i2c on D2 and D1 pin(i modified the ads library to make this)
  ads.setGain(GAIN_ONE);//+-4.096V
  //--**ADC

  if(readVin() < 3.5) {
    sendlowbat();
  }

}

int sended = 0;

void loop() {

  timer.run();

}

int sendData(float value, String pin){
  HTTPClient http;
  String payload = String("[\n") + value + String("\n]");
  Serial.println("\nstart:");
  Serial.println(http.begin(server_blynk, port_blynk, String("/") + auth + String("/pin/") + pin));
  http.addHeader("Content-Type", "application/json");//String("/") + auth + String("/pin/") + pin
  int codice_ritorno = http.sendRequest("PUT",(uint8_t *) payload.c_str(), payload.length());
  Serial.println(codice_ritorno);
  Serial.println(http.writeToStream(&Serial));
  http.end();
  Serial.println();
  return codice_ritorno;
}

int sendData(int value, String pin){
  HTTPClient http;
  String payload = String("[\n") + value + String("\n]");
  Serial.println("\nstart:");
  Serial.println(http.begin(server_blynk, port_blynk, String("/") + auth + String("/pin/") + pin));
  http.addHeader("Content-Type", "application/json");//String("/") + auth + String("/pin/") + pin
  int codice_ritorno = http.sendRequest("PUT",(uint8_t *) payload.c_str(), payload.length());
  Serial.println(codice_ritorno);
  Serial.println(http.writeToStream(&Serial));
  http.end();
  return codice_ritorno;
}

int sendVersion(String pin){
  HTTPClient http;
  String payload = String("[\n\"") + String(version) + String("\"\n]");
  Serial.println("\nstart:");
  Serial.println(http.begin(server_blynk, port_blynk, String("/") + auth + String("/pin/") + pin));
  http.addHeader("Content-Type", "application/json");//String("/") + auth + String("/pin/") + pin
  Serial.println(http.sendRequest("PUT",(uint8_t *) payload.c_str(), payload.length()));
  Serial.println(http.writeToStream(&Serial));
  http.end();
}

void sendlowbat(){
  HTTPClient http;
  Serial.println("\nstart:");
  Serial.println(http.begin(server_blynk, port_telegram, String("/lowbat") ));
  int codice_ritorno = http.GET();
  Serial.println(codice_ritorno);
  Serial.println(http.writeToStream(&Serial));
  http.end();
}

double readVin(){
  long sum = 0;
  for(int i = 0; i < numL;i++){
    int val = analogRead(A0);
    sum += val;
  }
  double avg = (double) sum / numL;
  double vin = (double) avg / (Kc * 1024.0f);
  return vin;
}

void aggiornaDormi(){
  if((WiFi.status() == WL_CONNECTED)) {    // wait for WiFi connection
    t_httpUpdate_return ret = ESPhttpUpdate.update(server_upd, port_upd, myName, version);//send to the update server the name of the device and the firmware version

    switch(ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES\n");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK\n");
        break;
    }
  }
  ESP.deepSleep(tsleep);
}

void spedisciDati(){//retrive sensor data and send to the blynk server
  //this function is orrible, i was probebly drunk, i will rewrite it one day
  sensors.requestTemperatures();

  double vin = readVin();
  float temp = sensors.getTempCByIndex(0);
  Serial.println(temp);

  sendData(int(millis()/1000), "V1");
  sendData(float(vin), "V0");
  sendVersion("V2");
  sendData((int) random(100), "V3");

  int16_t adc0, adc1, adc2, adc3;
  float Voltage0,Voltage1,Voltage2,Voltage3;

  adc0 = ads.readADC_SingleEnded(0);
  Voltage0 = (adc0 * 0.125)/1000;
  delay(20);

  adc1 = ads.readADC_SingleEnded(1);
  Voltage1 = (adc1 * 0.125)/1000;
  delay(20);

  adc2 = ads.readADC_SingleEnded(2);
  Voltage2 = (adc2 * 0.125)/1000;
  delay(20);

  adc3 = ads.readADC_SingleEnded(3);
  Voltage3 = (adc3 * 0.125)/1000;
  delay(20);

  adc0 = ads.readADC_SingleEnded(0);
  Voltage0 = (adc0 * 0.125)/1000;
  delay(20);

  adc1 = ads.readADC_SingleEnded(1);
  Voltage1 = (adc1 * 0.125)/1000;
  delay(20);

  adc2 = ads.readADC_SingleEnded(2);
  Voltage2 = (adc2 * 0.125)/1000;
  delay(20);

  adc3 = ads.readADC_SingleEnded(3);
  Voltage3 = (adc3 * 0.125)/1000;
  delay(20);

  Voltage2 *= 5.31969;

  Serial.print("(V Solare) AIN0: "); Serial.print(adc0);Serial.print("\tVin0: "); Serial.println(Voltage0);
  Serial.print("(V in) AIN0: "); Serial.print(adc2);Serial.print("\tVin0: "); Serial.println(Voltage2);
  Serial.print("(V LM35) AIN0: "); Serial.print(adc3);Serial.print("\tVin0: "); Serial.println(Voltage3);

  sendData(Voltage0, "V4");//Vin ADS

  sendData(Voltage1, "V30");//Vin pioggia

  sendData(Voltage2, "V5");//V solare

  sendData(Voltage3, "V26");//V LM35

  if(temp > -120){//failed read
    int send_code = sendData(temp, "V10");//DS18B20
    if(send_code == 200){
      sended = 1;
      aggiornaDormi();
    }
  }

  Serial.printf("Connection status: %d\n", WiFi.status());
}
