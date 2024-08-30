#include <Arduino.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

IPAddress ip(192,168,1,201);
IPAddress myDns(192,168,0,1);
EthernetClient client;
HttpClient http_client = HttpClient(client, "alfons.siebenlinden.net", 80);
char server[] = "alfons.siebenlinden.net";

#define RED_LED 2
#define GREEN_LED 5
#define YELLOW_LED 3
//Digital Pin 4 ist intern belegt! Darum Pin 5 für grün!

void setup() 
{

  byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0xE0, 0xBF };
  Serial.begin(9600);
  Ethernet.init(10);

  Serial.println("Initialize Ethernet with DHCP:");

   if (Ethernet.begin(mac) == 0) 
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
      Ethernet.begin(mac, ip, myDns);
      } 
   else 
      {
      Serial.print("  DHCP assigned IP ");
      Serial.println(Ethernet.localIP());
      }
   //delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("GET 7lenergy ...");
  http_client.get("/7lenergy/energy.json");

  // read the status code and body of the response
  //int statusCode = http_client.responseStatusCode();
  String response = http_client.responseBody();

  int start_at = response.lastIndexOf(":");
  String value = response.substring(start_at + 3, response.length() - 3);
  //Serial.println("extracted value " + value);
  
  //Serial.println("parsed value: ");
  //Serial.println(value.toInt());
  int energy_balance = value.toInt();
  Serial.print("VALUE=");
  Serial.print(energy_balance);
  Serial.print(",");
  if(energy_balance > 0 ) {
    Serial.println("GREEN");
    digitalWrite(2, LOW); //rot
    digitalWrite(5, HIGH); //grün
  
  }
  else {
    Serial.println("RED");
    digitalWrite(5, LOW);
    digitalWrite(2, HIGH);
    
  }
  delay(10000);
}