/*
 to visit the html page, go in your browser and type:
 http://arduinoyun.local/sd/switch

 */
#include <Wire.h>
#include <DS3231.h>
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <DHT.h>

/*VARIABILI*/

//Costanti
const uint8_t relay_1 = 12; //relay 1
#define DHTPIN 8   //DHT humidity sensor pin
#define DHTTYPE DHT22   //sensor type (DHT22)

//inizializations
YunServer server;
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
  YunClient client = server.accept();

  // There is a new client?
  if (client) {

    String command = client.readStringUntil('/');

    // is "relay" command?
    if (command == "relay") {

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

// read DHT22 sensor, can be rad every 2 seconds
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

  dht.begin();

  Bridge.begin();// Bridge startup
  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
}

void loop() {
  timeNow = RTClib::now();
  Serial.print(timeNow.year(), DEC);
  Serial.print('/');
  Serial.print(timeNow.month(), DEC);
  Serial.print('/');
  Serial.print(timeNow.day(), DEC);
  Serial.print(' ');
  Serial.print(timeNow.hour(), DEC);
  Serial.print(':');
  Serial.print(timeNow.minute(), DEC);
  Serial.print(':');
  Serial.print(timeNow.second(), DEC);
  Serial.println();

  readClients();

  if (timeNow.unixtime() - timeDHT.unixtime() >= 2) readDHT();
  delay(50); // Poll every 50ms
}


