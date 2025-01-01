#include <Arduino.h>
#include <WiFi.h>
#include <Ethernet.h>


int LED_NETWORK = 33;
int LED_BLUE = 32;
int LED_RED = 2;
int LED_YELLOW = 14;
int LED_GREEN = 15;

String WIFI_SSID = "0";
String WIFI_PWD = "fn0rd4223";

uint8_t MAC_eth[6];
uint8_t MAC_wifi[6];


enum Signal_Status
   {
   RED = 1,
   YELLOW = 2,
   GREEM = 3,
   };

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

void testLEDs()
   {
   int i=0;
   Serial.println("Testing LEDs...");
   while(i<15)
      {
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_BLUE, LOW);
      delay(30);
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_YELLOW, HIGH);
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_BLUE, LOW);
      delay(30);
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_BLUE, LOW);
      delay(30);
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_BLUE, HIGH);
      delay(30);
      digitalWrite(LED_BLUE, LOW);
      i++;
      }
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
   pinMode(LED_BLUE, OUTPUT);
   //get MAC Addresses
   esp_read_mac(MAC_eth, ESP_MAC_ETH);
   Serial.print("ETH MAC: "); for (int i = 0; i < 5; i++) {Serial.printf("%02X:", MAC_eth[i]);} Serial.printf("%02X\n", MAC_eth[5]);
   esp_read_mac(MAC_wifi, ESP_MAC_WIFI_STA);
   Serial.print("WIFI MAC: ");   for (int i = 0; i < 5; i++) {Serial.printf("%02X:", MAC_wifi[i]);} Serial.printf("%02X\n", MAC_wifi[5]);
//   Ethernet.init(33);
   }


void loop()
   {

   digitalWrite(LED_NETWORK, LOW); 
   testLEDs();
   delay(1000);
   if(InitWiFi())
      {
      Serial.print("...WiFi is ON, IP is:");
      Serial.println(WiFi.localIP());
      digitalWrite(LED_NETWORK, HIGH);
      }
   else
      {
      Serial.println("...Wifi is OFF");
      }

   if (InitLAN())
      {
      Serial.print("...LAN is ON, IP is: ");
      Serial.println(Ethernet.localIP());
      }
   else
      {
      Serial.println("...LAN is OFF");
      }



   delay(5000);
   }







/*   if(WiFi.status() == WL_CONNECTED)
      {
      Serial.println("Connected");
      Serial.println(
      }
   else
      {
      Serial.println("Not connected");
      delay(2000);
      }
*/
