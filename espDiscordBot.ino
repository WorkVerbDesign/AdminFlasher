/*  Discord bot code
 * 
 *  originally DiscordESPNotify by Eldin Zenderink 12-11-2017
 *  from: https://github.com/EldinZenderink/DiscordESPNotify/blob/master/DiscordESPNotify.ino
 *  
 *  uses codes.h to store seeecret info.
 *  
 *  modified by oh_bother (twitch.tv/oh_bother) 9/9/2018
 *  MIT License Applies
 */

/*
 *  THE GOAL:
 *    a string of absolutely retina shattering neopixles
 *    sit ontop of my pc's streaming monitor
 *    when I mess something up, leave a scene up, out of focus etc
 *    the moderator and cronies can blast me with a few quick
 *    attention getting light pulses with a command on a
 *    private discord room.
 *    I want this to be its own device, no servers. possibly
 *    its own discord account.
 *    
 *  MY CODE:
 *    if it's like the delkey it's going to take a lot ot essplain.
 *    it seems what this needs to do is use secure http to constantly
 *    ask for messages and gets the last one. 
 *    "poke the bear" method. 
 *  
 *  THE CIRCUIT:
 *    USB for power.
 *    8 neopixels sittin in a chiain. esp's 5v to the chain
 *    Wemos pin D4 (d2 silkscreen)  -> 1k ohm -> Din
 *    
 *  NOTES:
 *    be sure to be able to revoke tokens.
 */ 

#include "codes.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
//#include <FastLED.h>

//======Pins===========
//#define NUM_LEDS      8
//#define DATA_PIN      4

//======Settings======
#define WIFI_TRIES      10
#define WIFI_WAIT       500
#define WIFI_LONGWAIT   1000


//FASTLED_USING_NAMESPACE


/* codes.h variables, use these as if they were defined here.
  const char* ssid     = "YOURSSID";
  const char* password = "YOURPASSWORD";
  const String botToken = "YOURDISCORDBOTTOKEN";
  const String channelId = "YOURDISCORDCHANNELID";
*/
//==============Constants======================
const char* host = "discordapp.com";
const int httpsPort = 443; //ssl port

//==============Globals========================
String lastMessageId = ' ';
int state = 0;

//==============Library stuff==================
//fastLED
//CRGB leds[NUM_LEDS];

//espWIFI
WiFiClient client;

//==============Setup==============
void setup() {
  //Serial
  Serial.begin(115200);

  //LEDs
  //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

//=============Loop===============
void loop() {
  discoMachine();
  animRun();
}

//===========Main Functions======

//uses global state to run discord bot
//based on IRC bot
void discoMachine(){
  switch(state){
    case 0:
      wifiStarter();
      break;

    case 1:
      discoGet();
      break;

    case 2:
      parseMessage();
      break;
  }
  checkConnect();
}

//run the animation based on non delay timers
void animRun(){
//  patternList[commandNo].func();
//  
//  FastLED.show();
//  FastLED.delay(1000/fps);
//
//  EVERY_N_MILLISECONDS(20){ gHue++; }
}

//see if  stuff is still functioning for state machine
void checkConnect(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi borked!");
    state = 0;
  }
//else if(!client.connected()){
//    state = 1;
//}
}

//======Secondary Functions======
void wifiStarter(){
  Serial.println("connecting WiFi");
  WiFi.begin(ssid, password);
  int timeoutcount = 0;
  
  while(WiFi.status() != WL_CONNECTED){
    delay(WIFI_WAIT);
    Serial.print(".");
    
    if(timeoutcount > WIFI_TRIES){
      Serial.println(' ');
      Serial.println("trying again, 1sec");
      delay(WIFI_LONGWAIT);
      WiFi.begin(ssid, password);
      timeoutcount = 0;
    }
    timeoutcount++;
  }
  Serial.println("connected");
  state = 1;
}

//getting a message and connecting to the 
//host are all one operation?
void discoGet(){
  //JSON parser
  //Use arduinojson.org/assistant to compute the capacity.
  DynamicJsonDocument id(800);
//  DynamicJsonDocument doc;
  
  //get channel info
  String idRequest = "/api/channels/" + channelId;
  String received =   apiGet(idRequest);

  deserializeJson(id, received);
  JsonObject idOb = id.as<JsonObject>();
  String idGotten = idOb[("last_message_id")];
  Serial.println("here's da stuf: ");
  Serial.println(idGotten);

  if( idGotten != lastMessageId ){
    //get message STUFF HERE
    //WE'RE HERE    
    lastMessageId = idGotten
  }

//  String dataReceived;
//  String getchannelinfo = "/api/channels/" + channelId + "/messages";
//  dataReceived = apiGet(getchannelinfo);
//
//  deserializeJson(doc, dataReceived);
//  JsonObject obj = doc.as<JsonObject>();
//  String blather = obj[("content")];
//  Serial.println(blather);

  //rate limit check? read API you fuck.
  delay(6000);
}


//connects to the api and gets whatever requested endpoint is
//returns string containing everyone's favorite: JSON
String apiGet(String apiEndPoint){
  //use wifi client secure to make TLS connection
  WiFiClientSecure client;
  client.setNoDelay(1);

  //check if connected
  //does this initiate the connection?
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return "failed";
  }

  //request to client
  client.print(
    String("GET ") + apiEndPoint + " HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" +
    "Accept: */*\r\n" +
    "Authorization:Bot " + botToken + "\r\n" +
    "User-Agent:BotherBot (https://www.twitch.tv/oh_bother v1.0)\r\n" +
    "Content-Type: application/json\r\n" + 
    "Connection: close\r\n\r\n"
  );

  //Receive anything but the headers
  uint8_t receivedHeaders = 0;
  String received = " ";
  
  //replace this manky jawn with a JSON parse :ohbothEr:
  while(client.connected()){
    String line = client.readStringUntil('\n');
    Serial.println(line);
    if(line[0] == '{' || line[0] == '['){
      received = line;
      break;
    }
  }
  return received;
}

//parse the Json and do bot stuff (gross)
void parseMessage(){
  //nutin honey
}

//=====Bot chat functions=========


//======Helper Functions=========


//==========LED Animations=======
