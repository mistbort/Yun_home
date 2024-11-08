/*
 to visit the html page, go in your browser and type:
 http://arduinoyun.local/sd/Yun_control


 */
#include <Wire.h>
#include <DS3231.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeCLient.h>
#include <DHT.h>

/*VARIABILI*/

//Costanti
const uint8_t relay_1 = 8; //relay 1
const uint8_t relay_2 = 7; //relay 2
const uint8_t relay_3 = 4; //relay 3
const uint8_t relay_4 = 2; //relay 4
#define DHTPIN 12   //DHT humidity sensor pin
#define DHTTYPE DHT22   //sensor type (DHT22)

//inizializations
BridgeServer server;
DHT dht(DHTPIN, DHTTYPE); //dht humidity sensor object
DS3231 myRTC; //Declare a DS3231 object

//for time
DateTime timeNow;
DateTime timeLast;
DateTime timeDHT;

//audio

//humidity sensor
int chk;
float hum;  //% umidity
float temp; //temperature

/* FUNZIONI */

//read clients from server and set actions
void readClients(){
  // Get clients coming from server
  BridgeClient client = server.accept();

  // There is a new client?
  if (client) {
    String command = client.readStringUntil('/');
    Serial.print(command);
    if (command == "data"){  
      Serial.println("responding data");

      readDHT();
      //init JSON data
      client.println("Status:200");
      client.println("content-type:application/json");
      client.println();
      client.println("{");
      //timedata
      client.print("\"timedata\":\"");
      client.print(timeNow.year(), DEC);
      client.print("-");
      client.print(timeNow.month(), DEC);
      client.print("-");
      client.print(timeNow.day(), DEC);
      client.print(' ');
      client.print(timeNow.hour(), DEC);
      client.print(':');
      client.print(timeNow.minute(), DEC);
      client.print(':');
      client.print(timeNow.second(), DEC);
      client.println("\",");
      //temperature
      client.print("\"temperature\":\"");
      client.print(temp);
      client.println("\",");
      //humidity
      client.print("\"humidity\":\"");
      client.print(hum);
      //close
      client.println("\"}");   
    }


    // is "relay" command?
    if (command == "relay") {
      Serial.println("responding relay");
      int stat = client.parseInt();

      if (stat == 1) {
        digitalWrite(relay_1, LOW);    
      } 
      else {
        digitalWrite(relay_1, HIGH);
      }
    }

    // Close connection and free resources.
    client.stop();
  }
}

// read DHT22 sensor, can be read every 2 seconds
void readDHT(){
  hum = dht.readHumidity();
  temp= dht.readTemperature();
  //print on serial
  Serial.print("Umidità: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");
}


void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(relay_1,OUTPUT);
  digitalWrite(relay_1, HIGH);
  
  dht.begin();

  Bridge.begin();// Bridge startup
  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  //server.listenOnLocalhost();
  server.begin();
}

void loop() {
  timeNow = RTClib::now();
 

  readClients();

  //if (timeNow.unixtime() - timeDHT.unixtime() >= 2) readDHT();
  delay(50); // Poll every 50ms
}


