//Weather Station using NodeMCU ESP8266 & Thingspeak IoT Analytics.
// by Pranav Khatale
#include <dht.h> // Include DHT Header file for DHT sensor.
#define dht_apin 4// Analog Pin sensor is connected to pin D2.
#include "ThingSpeak.h"
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define light_pin 19
#define fan_pin 18
 
dht DHT;// Including library for DHT sensor.
const char *myWriteAPIKey = "6UKJBQNYITLFBMBN";     //  Enter your Write API key from ThingSpeak IoT Analytics Cloud platform.

const char *ssid =  "Suphason_2.4G";     // Replace with your Wi-Fi ssid.
const char *password =  "47sup365"; // // Replace with your Wi-Fi wpa2 key.
const char *server = "api.thingspeak.com"; //Initializing Server.
unsigned long myChannelNumber = 1539169;
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;
WiFiClient client;

LiquidCrystal_I2C lcd(0x27 , 20,4);
void setup() 
{
       lcd.begin();
       lcd.setCursor(0,0);
       // Program written in setup will run only once for connecting to Wi-Fi network.
       Serial.begin(115200); // Selecting the baud rate for Serial Communication.
       delay(500);
       // initialize digital pin LED_BUILTIN as an output.
       pinMode(LED_BUILTIN, OUTPUT);
       pinMode(light_pin, OUTPUT);
       pinMode(fan_pin, OUTPUT);
       WiFi.mode(WIFI_STA);   
       ThingSpeak.begin(client);  // Initialize ThingSpeak
       Serial.println("");
       Serial.println("WiFi connected."); //Prints on serial monitor if Wi-Fi gets connected sucessfully.
       lcd.setCursor(1,0);
       lcd.println("");
       lcd.println("WiFi connected."); //Prints on serial monitor if Wi-Fi gets connected sucessfully.
}
 
void loop() 
{
    if ((millis() - lastTime) > timerDelay) {
    
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
      lcd.println("\nConnected.");
    }
  DHT.read22(dht_apin); // Reading DHT sensor pin D2's data.
  
      float h = DHT.humidity; // Storing the humidity float value in variable h
      float t = DHT.temperature; // Storing the temperature float value in variable h
      
    ThingSpeak.setField(1, t);
    //ThingSpeak.setField(1, temperatureF);
    ThingSpeak.setField(2, h);

    if (t < 36.5){
      digitalWrite(light_pin , HIGH);
      digitalWrite(fan_pin , LOW);
    }

    if (t > 37.5){
      digitalWrite(light_pin , LOW);
      digitalWrite(fan_pin , HIGH);
    }

    if (h > 55){
      digitalWrite(light_pin , LOW);
      digitalWrite(fan_pin , HIGH);
    }

    if (h < 50.5){
      digitalWrite(light_pin , HIGH);
      digitalWrite(fan_pin , LOW);
    }

    lcd.setCursor(0,0);
    lcd.print("===========================");
    lcd.setCursor(0,1);
    lcd.print("Temperature : ");
    lcd.print(t);
    lcd.setCursor(0,2);
    lcd.print("Humidity (%): ");
    lcd.print(h);
    lcd.setCursor(0,3);
    lcd.print("===========================");
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }
  
  // thingspeak needs minimum 15 sec delay between on Cloud console updates for free subscription. 
  delay(1000);
}
