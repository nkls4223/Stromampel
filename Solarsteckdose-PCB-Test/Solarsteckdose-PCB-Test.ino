//#include <SPI.h>
//#include <Ethernet.h>
// Watch video here: https://www.youtube.com/watch?v=M4mVDnlnzSA

// UIP is a proper library for Arduino Nano Ethernet shield
// NOTE: UIPEthernet library is not needed if you are using Arduino UNO/Duemilanove/Mega/etc.
// UIPEthernet library is used for Arduino Nano Ethernet Shield

#include <UIPEthernet.h> // Used for Ethernet

// **** ETHERNET SETTING ****
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x78, 0xEE  };   
// Change the IP below to your subnet if you have any issues
//    IPAddress ip(192, 168, 0, 115);                                 
IPAddress ip(192, 168, 1, 115);

                        
EthernetServer server(80);

void setup() {
  Serial.begin(9600);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();

  if (client) 
  {  
    Serial.println("-> New Connection");

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;

    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) 
        {
          client.println("<html><title>Hello World!</title><body><h3>Hello World!</h3></body>");
          break;
        }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') 
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      } 
    }

    // give the web browser time to receive the data
    delay(10);

    // close the connection:
    client.stop();
    Serial.println("   Disconnected\n");
  }
}



/*void setup() {
  
  pinMode(3, OUTPUT); //grün
  pinMode(4, OUTPUT); //gelb
  pinMode(5, OUTPUT); //rot
  pinMode(6, OUTPUT); //Relais
}

void loop() {
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  delay(1000);
  
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
  delay(1000);
  
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
  delay(1000);
  
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  delay(1000);
}
*/