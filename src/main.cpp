#define myName "/ext/file.bin"
#define version "0.2.6"

#define numL 200

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SimpleTimer.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

int ONE_WIRE_BUS = 12;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

SimpleTimer timer;

char auth[] = "84950346964648d8ad9a1ccecd774691";
char ssid[] = "Bill Wi the Science Fi";
char pass[] = "t36wk67zzksmiy96";
const char server_upd[] = "192.168.1.124";
const char server_blynk[] = "192.168.1.124";
const int port_upd = 8090;
const int port_blynk = 8080;

//ESP8266WiFiMulti WiFiMulti;

const double Kc = 0.183105487805;

int sendData(float, String);
int sendData(int, String);
int sendVersion(String);
double readVin();

void spedisciDati();
void aggiornaDormi();

int flag = 0;

long t1 = 0;
long t2 = 0;

const long deltat = 1000*15;
const long deltat2 = 1000*2;

const long tsleep = 1000*1000*60*5;

long timedmax = 0;
long timedmin = 99999;

int sleepState = 0;

void setup(){

  if((readVin() < 3.2) && (readVin() > 3.1)) {
    flag = 1;
  }
  else if(readVin() < 3.1){
    flag = 1;
    ESP.deepSleep(1000*1000*60*30);
  }

  Serial.begin(115200);
  Serial.setDebugOutput(true);

  WiFi.mode(WIFI_STA);
  WiFi.begin("Bill Wi the Science Fi", "t36wk67zzksmiy96");

  while (WiFi.status() != WL_CONNECTED)  {
    delay(100);
    Serial.println(WiFi.status());
  }

  //http://blynk-cloud.com/auth_token/update/pin?value=value

  sensors.begin();

  timer.setInterval(deltat2, spedisciDati);
  timer.setInterval(deltat, aggiornaDormi);

}

int sended = 0;

void loop() {

  timer.run();

  /*long timed1 = millis();
  //terminal.print("Sum: ");terminal.print(sum);terminal.print("\tAvg: ");terminal.print(avg);terminal.print("\tVin: ");Serial.println(vin);
  /*if((millis() - t2) > deltat2){
    t2 = millis();

    sensors.requestTemperatures(); // Send the command to get temperatures

    String pin = "V10";

    /*Serial.print(String("/") + auth + String("/pin/") + pin); Serial.print("\t");
    Serial.print("/84950346964648d8ad9a1ccecd774691/pin/V14");Serial.print("\t");
    Serial.println(String("/84950346964648d8ad9a1ccecd774691/pin/V14").compareTo(String("/") + auth + String("/pin/") + pin));



    double vin = readVin();
    float temp = sensors.getTempCByIndex(0);
    Serial.println(temp);

    sendData(int(millis()/1000), "V1");
    sendData(float(vin), "V0");
    sendVersion("V3");
    sendData(flag, "V4");
    if(temp > -120){
      int send_code = sendData(temp, "V2");
      if(send_code == 200)
        sended = 1;
    //sendData(String(version), "V3");
    }

    Serial.printf("Connection status: %d\n", WiFi.status());
  }*/

  /*if(((millis() - t1) > deltat) || (sended)){
    t1 = millis();
      //ESP.deepSleep(tsleep, WAKE_RF_DEFAULT);
    if((WiFi.status() == WL_CONNECTED)) {    // wait for WiFi connection
      t_httpUpdate_return ret = ESPhttpUpdate.update(server_upd, port_upd, myName, version);

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
  }*/
/*
  long timed2 = millis();
  long dddddeltatime = timed2 - timed1;

  if(dddddeltatime > timedmax)
    timedmax = dddddeltatime;

  if(dddddeltatime < timedmin)
    timedmin = dddddeltatime;
*/
  //Serial.printf("\n******\nLoop time: %dms, Max loop: %dms, Min loop: %dms.\n",dddddeltatime, timedmax, timedmin);
}

int sendData(float value, String pin){
  HTTPClient http;
  String payload = String("[\n") + value + String("\n]");
  Serial.println("\nstart:");
  Serial.println(http.begin(server_blynk, port_blynk, String("/") + auth + String("/pin/") + pin));
  //"/84950346964648d8ad9a1ccecd774691/pin/V10"
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
  //"/84950346964648d8ad9a1ccecd774691/pin/V10"
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
  //"/84950346964648d8ad9a1ccecd774691/pin/V10"
  http.addHeader("Content-Type", "application/json");//String("/") + auth + String("/pin/") + pin
  Serial.println(http.sendRequest("PUT",(uint8_t *) payload.c_str(), payload.length()));
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
    t_httpUpdate_return ret = ESPhttpUpdate.update(server_upd, port_upd, myName, version);

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

void spedisciDati(){
  sensors.requestTemperatures(); // Send the command to get temperatures

  String pin = "V10";

  /*Serial.print(String("/") + auth + String("/pin/") + pin); Serial.print("\t");
  Serial.print("/84950346964648d8ad9a1ccecd774691/pin/V14");Serial.print("\t");
  Serial.println(String("/84950346964648d8ad9a1ccecd774691/pin/V14").compareTo(String("/") + auth + String("/pin/") + pin));
  */

  double vin = readVin();
  float temp = sensors.getTempCByIndex(0);
  Serial.println(temp);

  sendData(int(millis()/1000), "V1");
  sendData(float(vin), "V0");
  sendVersion("V3");
  sendData(flag, "V4");
  if(temp > -120){
    int send_code = sendData(temp, "V2");
    if(send_code == 200){
      sended = 1;
      aggiornaDormi();
    }
  //sendData(String(version), "V3");
  }

  Serial.printf("Connection status: %d\n", WiFi.status());
}
