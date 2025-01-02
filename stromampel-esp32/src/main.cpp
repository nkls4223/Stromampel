#include <Arduino.h>
#include <WiFi.h>
#include <Ethernet.h>
#include <HTTPClient.h>
#include <ESP32Time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

int LED_NETWORK = 33;
int LED_RED = 2;
int LED_YELLOW = 14;
int LED_GREEN = 15;
int RELAY_PIN = 32;
String WIFI_SSID = "0";
String WIFI_PWD = "fn0rd4223";

uint8_t MAC_eth[6];
uint8_t MAC_wifi[6];

ESP32Time rtc(0);

//Time for hysteresis
int HystTime=900;

//EpochTime when Hysteresis timer was started
unsigned long HystTimeStarted=0;

enum NetworkConnection
   {
   DISCONNECTED = 0,
   WIFI = 1,
   LAN = 2,
   };
NetworkConnection connection = DISCONNECTED;



enum SignalStatus
   {
   OFF = 0,
   RED = 1,
   YELLOW = 2,
   GREEN = 3,
   };
SignalStatus lstatus = OFF;

enum RelayStatus
   {
   RELAYOFF = 0,
   RELAYON = 1,
   };
RelayStatus rstatus = RELAYOFF;


bool InitWiFi()
   {
   if(WiFi.status() == WL_CONNECTED)
      {
      return(1);
      }
   else
      {
      WiFi.mode(WIFI_STA);
      WiFi.begin(WIFI_SSID, WIFI_PWD);
      delay(1000);
      if(WiFi.status() == WL_CONNECTED)
         {
         return(1);
         }
      else
         {
         return(0);
         }
      }
   }

bool InitLAN()
   {
   if(Ethernet.localIP()!=0)
      {
      return(1);
      }
   else
      {
      Ethernet.begin(MAC_eth);
      delay(1000);
         if(Ethernet.localIP()!=0)
            {
            return(1);
            }
         else
            {
            return(0);
            }
      }
   }

SignalStatus GetData()
   {
   SignalStatus status; 
   int energy_balance;
   switch(connection)
      {
      case DISCONNECTED:
         {
         Serial.println("GetData(): Network DISCONNECTED");
         return(OFF);
         break;
         }
      case WIFI:
         {
         Serial.println("GetData(): Network WIFI");
         Serial.println("Requesting Data via WiFi...");
         WiFiClient wificlient;
         HTTPClient httpclient;
//         String serverPath = "http://alfons.siebenlinden.net/7lenergy/energy.json";
         String serverPath = "http://alfons.siebenlinden.net/7lenergy/energy_test.json";
         httpclient.begin(wificlient, serverPath.c_str());
         int httpResponseCode = httpclient.GET();
         if (httpResponseCode>0) 
            {
            String payload = httpclient.getString();
            int start_at = payload.lastIndexOf(":");
            String value = payload.substring(start_at + 3, payload.length() - 3);
            energy_balance = value.toInt();
            break;
            }
         else
            {
            return(OFF);
            break;
            } 
         }
      case LAN:
         {
         Serial.println("GetData(): Network LAN");

         //temporary, fix later
         return(OFF);
         break;
         }
      }

   Serial.print("Energy balance:");
   Serial.println(energy_balance);

   if((energy_balance > 0) && (energy_balance < 1000 )) 
      {
      Serial.println("Signal is YELLOW");
      status=YELLOW;
      return(status);
      }

   if(energy_balance >= 1000) 
      {
      Serial.println("Signal is GREEN");
      status=GREEN;
      return(status);
      }

   if(energy_balance <= 0)
      {
      Serial.println("Signal is RED");
      status=RED;
      return(status);
      }

   }

void SetLEDSignal(SignalStatus status)
   {
   Serial.print("SetLEDStatus(): status is ");
   Serial.println(status);
   switch(status)
      {
      case GREEN:
         {
         Serial.println("SetLEDStatus(): turning green LED on");
         digitalWrite(LED_GREEN, HIGH);
         digitalWrite(LED_YELLOW, LOW);
         digitalWrite(LED_RED, LOW);
         break;
         }
      case YELLOW:
         {
         Serial.println("SetLEDStatus(): turning yellow LED on");
         digitalWrite(LED_GREEN, LOW);
         digitalWrite(LED_YELLOW, HIGH);
         digitalWrite(LED_RED, LOW);
         break;
         }
      case RED:
         {
         Serial.println("SetLEDStatus(): turning red LED on");
         digitalWrite(LED_GREEN, LOW);
         digitalWrite(LED_YELLOW, LOW);
         digitalWrite(LED_RED, HIGH);
         break;
         }
      case OFF:
         {
         Serial.println("SetLEDStatus(): turning all LEDs off");
         digitalWrite(LED_GREEN, LOW);
         digitalWrite(LED_YELLOW, LOW);
         digitalWrite(LED_RED, LOW);
         break;
         }


      }

   }

void SetRelayStatus(SignalStatus LEDstatus)
   {
//   DateTime NowTime=rtc.now();
// int HystTime=900;
// 24
// 25 //EpochTime when Hysteresis timer was started
// 26 long HystTimeStarted;

   

   switch(LEDstatus)
       {
       case RED:
          {
          if((rtc.getLocalEpoch()-HystTimeStarted)>HystTime)
             {
             digitalWrite(RELAY_PIN, LOW);
             rstatus=RELAYOFF;
             break;
             }
          }
       case YELLOW:
          {
          if((rtc.getLocalEpoch()-HystTimeStarted)>HystTime)
             {
             digitalWrite(RELAY_PIN, LOW);
             rstatus=RELAYOFF;
             break;
             }
          }
       case GREEN:
          {
          digitalWrite(RELAY_PIN, HIGH);
	  rstatus=RELAYON;
          HystTimeStarted=rtc.getLocalEpoch();
          break;
          }
       case OFF:
          {
          if((rtc.getLocalEpoch()-HystTimeStarted)>HystTime)
             {
             digitalWrite(RELAY_PIN, LOW);
             rstatus=RELAYOFF;
             break;
             }
          }
       }
   }


void InitRTC()
   {
   Serial.println("Initializing RTC...");
   WiFiUDP ntpUDP;
   NTPClient timeClient(ntpUDP, "ptbtime1.ptb.de", 3600);
   timeClient.begin();
   timeClient.update();
   rtc.setTime(timeClient.getEpochTime());
   String formattedTime = timeClient.getFormattedTime();
   Serial.println(formattedTime);
   }


/*=======================================================================*/

void setup()
   {

   Serial.begin(9600);
   delay(5000);

   pinMode(LED_NETWORK, OUTPUT);
   pinMode(LED_RED, OUTPUT);
   pinMode(LED_YELLOW, OUTPUT);
   pinMode(LED_GREEN, OUTPUT);
   pinMode(RELAY_PIN, OUTPUT);
   //get MAC Addresses
   esp_read_mac(MAC_eth, ESP_MAC_ETH);
   Serial.print("ETH MAC: "); for (int i = 0; i < 5; i++) {Serial.printf("%02X:", MAC_eth[i]);} Serial.printf("%02X\n", MAC_eth[5]);
   esp_read_mac(MAC_wifi, ESP_MAC_WIFI_STA);
   Serial.print("WIFI MAC: ");   for (int i = 0; i < 5; i++) {Serial.printf("%02X:", MAC_wifi[i]);} Serial.printf("%02X\n", MAC_wifi[5]);
   InitWiFi();
   InitLAN();
   InitRTC();
   }


void loop()
   {

   digitalWrite(LED_NETWORK, LOW); 
   NetworkConnection conn = DISCONNECTED;;
   delay(1000);

   if(InitWiFi())
      {
      Serial.print("...WiFi is ON, IP is:");
      Serial.println(WiFi.localIP());
      digitalWrite(LED_NETWORK, HIGH);
      connection=WIFI;
      }
   else
      {
      Serial.println("...Wifi is OFF");
      }

/*   if (InitLAN())
      {
      Serial.print("...LAN is ON, IP is: ");
      Serial.println(Ethernet.localIP());
      conn= LAN;
      }
   else
      {
      Serial.println("...LAN is OFF");
      }
*/

//   Serial.print("Time is: ");
   //ESP32Time tnow = rtc.now();

   Serial.print("Time is: ");
   Serial.println(rtc.getDateTime());
   SignalStatus status = GetData();
   Serial.print("Signal Status: ");
   Serial.println(status);
   SetLEDSignal(status);
   SetRelayStatus(status);

   delay(5000);
   }







/*   if(WiFi.status() == WL_CONNECTED)
      {
      Serial.println("Connected");
      Serial.println(
      }
   else
      {
      Serial .println("Not connected");
      delay(2000);
      }
*/

