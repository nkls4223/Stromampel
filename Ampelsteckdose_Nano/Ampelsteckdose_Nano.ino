#include <Arduino.h>
#include <UIPEthernet.h>
#include <ArduinoHttpClient.h>

//IPAddress ip(192,168,1,207);
//IPAddress myDns(192,168,0,1);
EthernetClient client;
HttpClient http_client = HttpClient(client, "alfons.siebenlinden.net", 80);
char server[] = "alfons.siebenlinden.net";

#define RED_LED 5
#define GREEN_LED 3
#define YELLOW_LED 4
#define RELAIS 6
//Digital Pin 4 ist intern belegt! Darum Pin 5 für grün!


/*unsigned long relaydelay=000; //delay for the relay
unsigned long lasttimer;
unsigned long timepassed;  
int relais_is_on=0;
*/


unsigned long ms_since_relais_changed=0;
unsigned long ms_since_time_update=0;
int relais_state=0;
unsigned long ms_power_cycle=300000;
int power_cycle_granted=1;
int led_state=0;
int first_run=1;

void setup() 
{

  pinMode(RED_LED,OUTPUT);
  pinMode(GREEN_LED,OUTPUT);
  pinMode(YELLOW_LED,OUTPUT);
  pinMode(RELAIS,OUTPUT);

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
      //Ethernet.begin(mac, ip, myDns);
      } 
   else 
      {
      Serial.print("  DHCP assigned IP ");
      Serial.println(Ethernet.localIP());
      }
   //delay(1000);
  

}

void loop() 
  {
  Serial.print("GET 7lenergy ...");
  http_client.get("/7lenergy/energy.json");
  String response = http_client.responseBody();
  int start_at = response.lastIndexOf(":");
  String value = response.substring(start_at + 3, response.length() - 3);
  int energy_balance = value.toInt();
  /*Serial.print("VALUE=");
  Serial.print(energy_balance);
  Serial.print(",");*/

  Serial.println("RESPONSE");
  Serial.println(response);
  Serial.println("VALUE");
  Serial.println(value);
  Serial.println("ENERGYBALANCE");
  Serial.println(energy_balance);


  ms_since_time_update=ms_since_time_update + (millis()-ms_since_time_update);
  ms_since_relais_changed=ms_since_relais_changed + ms_since_time_update;
  ms_since_time_update=0;


  if (ms_since_relais_changed >= ms_power_cycle)
    {
    power_cycle_granted=1;
    }
  else
    {
    power_cycle_granted=0;  
    }

  if (first_run==1)
    {
    power_cycle_granted=1;  
    }
  Serial.println("");
  Serial.print("preset for power cycle delay: ");
  Serial.println(ms_power_cycle);
  Serial.print("First Run: ");
  Serial.println(first_run);
  Serial.print("Time since relais changed: ");
  Serial.println(ms_since_relais_changed);
  Serial.print("Power Cycle Granted: ");
  Serial.println(power_cycle_granted);


 

  if(energy_balance <= 0) 
    {
    if ((power_cycle_granted==1) && (relais_state==1))
      {
      digitalWrite(RELAIS, LOW);
      relais_state=0;
      ms_since_relais_changed=0;
      }
    Serial.println("RED");
    led_state=0;
    digitalWrite(RED_LED, HIGH); 
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW); 
    }
  
  
  if((energy_balance > 0) && (energy_balance < 1000 )) 
    {
    if ((power_cycle_granted==1) && (relais_state==1))
      {
      digitalWrite(RELAIS, LOW);
      relais_state=0;
      ms_since_relais_changed=0;
      }
    Serial.println("YELLOW");
    led_state=1;
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    }

  if(energy_balance >= 1000) 
    {
    if (relais_state==0)
      {
      digitalWrite(RELAIS, HIGH);
      relais_state=1;
      ms_since_relais_changed=0;
      first_run=0;
      }
    
    Serial.println("GREEN");
    led_state=2;
    digitalWrite(RED_LED, LOW); 
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    }

    Serial.print("Relais State: ");
    Serial.println(relais_state);

/*if((energy_balance > 0) && (energy_balance < 1000 )) {
    Serial.println("YELLOW");
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    timepassed=millis()-lasttimer;
    if((relais_is_on == 1) && (timepassed <= relaydelay))
      {
      }
    else 
      {
      digitalWrite(RELAIS, LOW);
      relais_is_on=0;
      }

  }
if(energy_balance >= 1000) {
    Serial.println("GREEN");
    digitalWrite(RED_LED, LOW); 
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RELAIS, HIGH);
    relais_is_on=1;
  }
if(energy_balance <= 0) {
    Serial.println("RED");
    digitalWrite(RED_LED, HIGH); 
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW); 
        timepassed=millis()-lasttimer;
    if((relais_is_on == 1) && (timepassed <= relaydelay))
      {
      }
    else 
      {
      digitalWrite(RELAIS, LOW);
      relais_is_on=0;
      }
    lasttimer=millis();
  }
 */     
  
  delay(10000);
}