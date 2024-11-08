#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "RTClib.h"
#include <WiFiUdp.h>
#include <NTPClient.h>

#define _network_status_led D3
#define _led_red D5
#define _led_yellow D6
#define _led_green D7
#define _relay_pin D0

const char* ssid = "0";
const char* password = "fn0rd4223";

unsigned long previousMillis = 0;
unsigned long interval = 10000;

TimeSpan HystSetTimeSpan(0,0,15,0);
// TimeSpan HystActTimeSpan();
// DateTime HystStartDateTime();
DateTime HystEndDateTime;
bool RelayOn = false;

WiFiUDP ntpUDP;

//NTPClient timeClient(ntpUDP, "ptbtime1.ptb.de", 19800, 60000);
NTPClient timeClient(ntpUDP, "ptbtime1.ptb.de", 3600);

RTC_DS1307 rtc;

//HttpClient client(WiFi);

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);


}

void testLEDs()
   {
   Serial.println("Testing LEDs...");
   digitalWrite(_led_red, HIGH);
   digitalWrite(_led_yellow, LOW);
   digitalWrite(_led_green, LOW);
   delay(1000);
   digitalWrite(_led_red, LOW);
   digitalWrite(_led_yellow, HIGH);
   digitalWrite(_led_green, LOW);
   delay(1000);
   digitalWrite(_led_red, LOW);
   digitalWrite(_led_yellow, LOW);
   digitalWrite(_led_green, HIGH);
   delay(1000);
   digitalWrite(_led_green, LOW);
   }

void SwitchRelay(bool value)
  {
  Serial.print("Relay Value is ");
  Serial.println(value);
  DateTime NowTime=rtc.now();
  switch (value)
    {
    case true:
      {
      if (RelayOn)
        {
        }
      else
        {
        Serial.println("Turning Relay ON");
        digitalWrite(_relay_pin, HIGH);
        RelayOn = true;
        HystEndDateTime = NowTime + HystSetTimeSpan;
        break;
        }
      }
    case false:
      {
      if (RelayOn)
        {
        if (NowTime < HystEndDateTime)
          {
          }
        else
          {
          Serial.println("Turning Relay OFF");
          digitalWrite(_relay_pin, LOW);
          RelayOn = false;
          break;
          }
        }
      else
        {
        }
   } 
    } 
  }

void setup() {
  delay(1000);
  Serial.begin(9600);
  delay(100);
  Serial.println("Device started!");

  pinMode(_led_red, OUTPUT);
  pinMode(_led_yellow, OUTPUT);
  pinMode(_led_green, OUTPUT);
  pinMode(_relay_pin, OUTPUT);
  testLEDs();


  pinMode(_network_status_led, OUTPUT);

  digitalWrite(_network_status_led, LOW);

  initWiFi();
//  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());

  Serial.println("Starting NTP Client");
  timeClient.begin();

  Serial.println("Updating NTP Client");
  timeClient.update();
  rtc.begin();

  Serial.println("Adjusting Real Time Clock");

  unsigned long unix_epoch = timeClient.getEpochTime();  // Get epoch time
  rtc.adjust(DateTime(unix_epoch));


}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >=interval){
    switch (WiFi.status()){
      case 3:
        Serial.println("Connection successfully established");
        digitalWrite(_network_status_led, HIGH);
        break;
      default:
        Serial.println("No connection");
        digitalWrite(_network_status_led, LOW);
        break;
    }
    Serial.printf("Connection status: %d\n", WiFi.status());
//    Serial.print("RRSI: ");
//    Serial.println(WiFi.RSSI());

    Serial.print("Time is: ");
    DateTime tnow = rtc.now();


    Serial.print(tnow.year(), DEC);
    Serial.print('/');
    Serial.print(tnow.month(), DEC);
    Serial.print('/');
    Serial.print(tnow.day(), DEC);
    Serial.print(" ");
    Serial.print(tnow.hour(), DEC);
    Serial.print(':');
    Serial.print(tnow.minute(), DEC);
    Serial.print(':');
    Serial.print(tnow.second(), DEC);
    Serial.println();

    // Get Power data from Alfons
    if(WiFi.status()== WL_CONNECTED)
      {
      WiFiClient client;
      HTTPClient http;
//      String serverPath = "http://alfons.siebenlinden.net/7lenergy/energy.json";
      String serverPath = "http://alfons.siebenlinden.net/7lenergy/energy.json";
      http.begin(client, serverPath.c_str());
      int httpResponseCode = http.GET();
      if (httpResponseCode>0) 
        {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
//        Serial.println(payload);
        int start_at = payload.lastIndexOf(":");
        String value = payload.substring(start_at + 3, payload.length() - 3);
        int energy_balance = value.toInt();
        Serial.println(energy_balance);


        if((energy_balance > 0) && (energy_balance < 1000 )) 
          {
          Serial.println("Signal is YELLOW");
          digitalWrite(_led_red, LOW);
          digitalWrite(_led_yellow, HIGH);
          digitalWrite(_led_green, LOW);
          SwitchRelay(false);
          }

        if(energy_balance >= 1000) 
          {
          Serial.println("Signal is GREEN");
          digitalWrite(_led_red, LOW); 
          digitalWrite(_led_yellow, LOW);
          digitalWrite(_led_green, HIGH);
	  SwitchRelay(true);
          }

        if(energy_balance <= 0) 
          {
          Serial.println("Signal is RED");
          digitalWrite(_led_red, HIGH); 
          digitalWrite(_led_yellow, LOW);
          digitalWrite(_led_green, LOW); 
	  SwitchRelay(false);
          }
        }
      else 
        {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        }
      }
    else
      {
      Serial.println("WiFi Disconnected, cant get data");
      }


    previousMillis = currentMillis;

    }
}
