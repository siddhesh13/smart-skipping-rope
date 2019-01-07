
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#include <WiFiManager.h> 
#include <EEPROM.h>
#include "play.h"


#define outputA 4
#define outputB 5
#define button 14
#define led 2

int counter = 0; 
int aState;
int aLastState;
int skipCount =0;
int prevskipCount =0; 
int skips=0; 
//unsigned long longPress= 6000;
//unsigned long currentMillis=0;
//unsigned long prevMillis=0;
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
bool buttonPressed = false;
bool settingMode = false;


int knockReading = 0;      // variable to store the value read from the sensor pin
const int knockSensor = A0; // the piezo is connected to analog pin 0
const int threshold = 15;  // threshold value to decide when the detected sound is a knock or not
//int score = 0;
boolean playing = false;

const char* Weight = "";
float met=8.8;
int skippingRate=0;
float caloriesBurnt;
int wght=50;
unsigned long startTime=0;
String apiKey = "*****************"; 
int calories=0;
bool thingspeak = false;
/////////////////////////
// Network Definitions //
/////////////////////////
const IPAddress AP_IP(192, 168, 1, 1);
const char* AP_SSID = "Smarr Skipping Rope";
boolean SETUP_MODE;
String SSID_LIST;
DNSServer DNS_SERVER;
//ESP8266WebServer server(80);

/////////////////////////
// Device Definitions //
/////////////////////////
String DEVICE_TITLE = "Smart Skipping Rope";

#define USE_SERIAL Serial


ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Ready!");
        }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);

            if(payload[0] == '#') {
                // we get RGB data
                USE_SERIAL.printf(" weight %s\n",  payload);
                String w="";
                w = String((const char *) &payload[1]);
                wght = w.toInt();
            }
            break;
    }
}

void logToThingspeak(String key, int burntCalories, int skipRate, int skips)
     {
        const char* host = "api.thingspeak.com";
      
         Serial.print("Connecting to "); Serial.print(host);

         WiFiClient client;
         int retries = 5;
         while(!client.connect(host, 80) && (retries-- > 0)) 
              {
                Serial.print(".");
              }
         Serial.println();
         if (!client.connected())
            {
              Serial.println("Failed to connect, going back to sleep");
              //return false;
            }
  
         String postStr = key;
         postStr += "&field1=";
         postStr += String(skips);
         postStr += "&field2=";
         postStr += String(skipRate);
         postStr += "&field3=";
         postStr += String(burntCalories);
         postStr += "\r\n\r\n";

         client.print("POST /update HTTP/1.1\n");
         client.print("Host: api.thingspeak.com\n");
         client.print("Connection: close\n");
         client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
         client.print("Content-Type: application/x-www-form-urlencoded\n");
         client.print("Content-Length: ");
         client.print(postStr.length());
         client.print("\n\n");
         client.print(postStr);
 
       for(byte i=0; i<7;i++){
        digitalWrite(led,LOW);
        delay(250);
        digitalWrite(led,HIGH);
        delay(250);
        
       }
        if(!client.available()) 
          {
            Serial.println("No response");
           // return false;
          }
        Serial.println(F("disconnected"));
      }



void setup() {
   pinMode (outputA,INPUT);
   pinMode (outputB,INPUT);
   pinMode (button, INPUT_PULLUP);
   pinMode (led,OUTPUT);
   // Reads the initial state of the outputA
   aLastState = digitalRead(outputA);
      
    USE_SERIAL.begin(115200);
    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(500);
    }
   
    WiFiManager wifiManager;
    // wifiManager.resetSettings();
    wifiManager.autoConnect("Smart Skipping Rope");
    Serial.println("connected to Smart Skipping Rope)");
    Serial.println( WiFi.SSID().c_str());
    Serial.println(WiFi.psk().c_str());
    wifiManager.setConfigPortalTimeout(180);

    // start webSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    if(MDNS.begin("esp8266")) {
        USE_SERIAL.println("MDNS responder started");
        USE_SERIAL.println(WiFi.localIP());
    }

    // handle index
    server.on("/", []() {
        // send index.html
        String s = MAIN_page;
        server.send(200, "text/html", s);
    });
    server.begin();
    // Add service to MDNS
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);
}

void loop() {
 
    webSocket.loop();
    server.handleClient();
    //knockReading = analogRead(A0);
    //delay(1);
    buttonState = digitalRead(button);
    
    aState = digitalRead(outputA); // Reads the "current" state of the outputA
    // If the previous and the current state of the outputA are different, that means a Pulse has occured
    if(aState != aLastState){     
      // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
      if(digitalRead(outputB) != aState){ 
         counter ++;
   
       } 
      else{
        }
      
     } 
    aLastState = aState; // Updates the previous state of the outputA with the current state
    delay(18);
    
    if(buttonState != lastButtonState){
      if(buttonState == HIGH){
        buttonPushCounter++;  
      }
    }
    if(buttonPushCounter %2 ==0){
      //Serial.print("Activity Started");
      digitalWrite(led,HIGH);
      // If there is a bounce, send a message to websocket
      if(counter >= 5){
        skipCount = skipCount + 1;
        counter = 0;
        playing = true;
        //Serial.print("Counts: ");
        //Serial.println(skipCount);
      } 
      if(skipCount==1){
         thingspeak = true;
         startTime = millis();
      }
            
      if(skippingRate < 100) met=8.8;
      else if(skippingRate >= 100 && skippingRate < 120) met=11.8;
      else if(skippingRate >= 120 && skippingRate < 160) met=12.3;  
      if(skipCount%21 == 0){
        skippingRate = (60000*skipCount)/(millis()-startTime);
        String sRate = "S"+String(skippingRate);
        //Serial.print("Skipping rate sent: ");
        //Serial.println(sRate);
        webSocket.sendTXT(0,sRate);
      }
      else if(skipCount%50 == 0){
        caloriesBurnt = float((millis()-startTime)/60000.0)*met*3.5*float(wght)/200.0;
        calories = caloriesBurnt;
        String cb = "$"+String(calories);
        webSocket.sendTXT(0,cb);
      }
      else {  
       //last_bounce = millis();
       String thisString = String(skipCount);
       webSocket.sendTXT(0,thisString);
       prevskipCount = skipCount;
      }
      //delay(10);
    } 
 
    else{
     //Serial.print("Activity Stop");
     digitalWrite(led,LOW);
     webSocket.sendTXT(0,"Take some rest");
     if(thingspeak == true){
       skippingRate = (60000*skipCount)/(millis()-startTime);
       caloriesBurnt = float((millis()-startTime)/60000.0)*met*3.5*float(wght)/200.0;
       logToThingspeak(apiKey, calories, skippingRate, skipCount);
       thingspeak = false;  
      }
     skipCount = 0;
     skips=0;
     playing=false;
     delay(100);
    }
  lastButtonState = buttonState; 
}

