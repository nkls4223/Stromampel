#include <Arduino.h>
#include <WiFi.h>
#include <Ethernet.h>

String WIFI_SSID = "0";
String WIFI_PWD = "fn0rd4223";

uint8_t MAC_eth[6];
uint8_t MAC_wifi[6];

EthernetClient LANclient;

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
      if(Ethernet.begin(MAC_eth))
         {
         return(1);
         }
      else
         {
         return(0);
         }
      }
   }



void setup()
   {
   Serial.begin(9600);
   delay(5000);

   //get MAC Addresses
   esp_read_mac(MAC_eth, ESP_MAC_ETH);
   Serial.print("ETH MAC: "); for (int i = 0; i < 5; i++) {Serial.printf("%02X:", MAC_eth[i]);} Serial.printf("%02X\n", MAC_eth[5]);
   esp_read_mac(MAC_wifi, ESP_MAC_WIFI_STA);
   Serial.print("WIFI MAC: ");   for (int i = 0; i < 5; i++) {Serial.printf("%02X:", MAC_wifi[i]);} Serial.printf("%02X\n", MAC_wifi[5]);

   }


void loop()
   {

   if(InitWiFi())
      {
      Serial.print("...WiFi is ON, IP is:");
      Serial.println(WiFi.localIP());
      }
   else
      {
      Serial.println("...Wifi is OFF");
      }

   if (InitLAN)
      {
      Serial.print("...LAN is ON, IP is: ");
      Serial.println(Ethernet.localIP());
      }
   else
      {
      Serial.println("...LAN is OFF");
      }



/*   if(WiFi.status() == WL_CONNECTED)
        {
        Serial.println("Connected");
        }
     else
        {
        Serial.println("Not connected");
        delay(2000);
        }
*/
   Serial.println("Loop has looped");

/*   if (Ethernet.begin(mac) == 0) 
      {
      Serial.println("Failed to configure Ethernet using DHCP");
      if (Ethernet.hardwareStatus() == EthernetNoHardware) 
         {
         Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
         while (true) 
            {
            delay(1);
            }
         }
      if (Ethernet.linkStatus() == LinkOFF) 
         {
         Serial.println("Ethernet cable is not connected.");
         }
      //Ethernet.begin(mac, ip, myDns);
      } 
   else 
      {
      Serial.print("  DHCP assigned IP ");
      Serial.println(Ethernet.localIP());
      }
*/  
   delay(1000);
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
