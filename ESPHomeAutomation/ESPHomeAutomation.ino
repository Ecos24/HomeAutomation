// Libraries.
#include <pins_arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

// Details for the wifi.
const char *ssid = "************";
const char *password = "*************";
const char *hostName = "***************";

// Server refference with port used for the server.
ESP8266WebServer server (YourPortNumber);

#define LOW 1
#define HIGH 0
#define ON 1
#define OFF 0
#define SCREEN 1
#define MAINROUTER 2
#define SPEAKER 3
#define PERIPHERAL 4

const char *authKey = "***********";

// Relay Points Details
// Four point Relay
#define screen D1
#define mainRouter D2
#define speaker D5
#define peripheral D6


void setup(void)
{
  // pin Modes.
  pinMode(screen, OUTPUT);
  pinMode(mainRouter, OUTPUT);
  pinMode(speaker, OUTPUT);
  pinMode(peripheral,OUTPUT);
  
  digitalWrite(screen, LOW);
  digitalWrite(speaker, LOW);
  digitalWrite(mainRouter, LOW);
  digitalWrite(peripheral, LOW);
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Hello");
  Serial.print("Eshtablishing connection with ");
  Serial.print(ssid);
  Serial.println(".");

  // Wait for the connection to eshtablish.
  Serial.println("");
  while( WiFi.status() != WL_CONNECTED )
  {
    delay(100);
    Serial.print(".");
  }

  // Connected to the network, printing details.
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP Address : ");
  Serial.print(WiFi.localIP());

  if( MDNS.begin(hostName) )
  {
    Serial.println("MDNS responder started" );
  }

  // Server Path Handlers.
  server.on("/", handleRoot);
  server.on("/customPath", HTTP_POST, sendStatus);
  server.on("/customPath", HTTP_POST, allAppliancesOff);
  server.on("/customPath", HTTP_POST, actionScreen);
  server.on("/customPath", HTTP_POST, actionSpeaker);
  server.on("/customPath", HTTP_POST, actionMainRouter);
  server.on("/customPath", HTTP_POST, actionPeripheral);
  server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );
  } );
  server.onNotFound(handleNotFound);

  // Starting server.
  server.begin();
  Serial.println("HTTP Server started");

  // Put code to revert to previous state.
}

void loop()
{
  server.handleClient();
}

String validateRequest(bool onOffFlag)
{
  if(server.args() > 0)
  {
    if(server.hasArg("plain"))
    {
      String data = server.arg("plain");
      StaticJsonBuffer<100> jBuffer;
      JsonObject& details = jBuffer.parseObject(data);
      if(details.containsKey("id"))
      {
        String id = details["id"];
        if(id.equals(authKey))
        {
          // For satus & allOff
          if(!onOffFlag)
          {
            return "";
          }
          /////////////////////
          
          if(details.containsKey("state"))
          {
            return details["state"];
          }
          else
          {
            server.send(909, "text/plain", "Bad Request");
          }
        }
        else
        {
          server.send(908, "text/plain", "Authentication Failed");
        }
      }
      else
      {
        server.send(907, "text/plain", "Invalid Request");
      }
    }
    else
    {
      server.send(906, "text/plain", "Invalid Request");
    }
  }
  else
  {
    server.send(906, "text/plain", "Invalid Request");
  }
}

// ShutDown All.
void allAppliancesOff()
{
  validateRequest(false);
  digitalWrite(screen, LOW);
  digitalWrite(speaker, LOW);
  digitalWrite(mainRouter, LOW);
  digitalWrite(peripheral, LOW);

  server.send(200, "text/plain", "Turned Everythig Off");
}

// Method to status()
void sendStatus()
{
  validateRequest(false);
  
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& currentStatus = jsonBuffer.createObject();
  currentStatus.set("Screen",!digitalRead(screen));
  currentStatus.set("Speaker",!digitalRead(speaker));
  currentStatus.set("MainRouter",!digitalRead(mainRouter));
  currentStatus.set("Peripheral",!digitalRead(peripheral));

  String json;
  currentStatus.prettyPrintTo(json);
  
  server.send(200, "application/json", json);
}

// ON OFF Function's Started.
void actionScreen()
{
  int state = validateRequest(true).toInt();
  switch(state)
  {
    case ON:
      if(digitalRead(screen) != HIGH)
      {
        Serial.println("Turning Screen On");
        digitalWrite(screen, HIGH);
        server.send(200, "text/plain", "Turned Screen On");
      }
      server.send(200, "text/plain", "Screen Already On");
      break;
    case OFF:
      if(digitalRead(screen) != LOW)
      {
        Serial.println("Turning Screen Off");
        digitalWrite(screen, LOW);
        server.send(200, "text/plain", "Turned Screen Off");
      }
      server.send(200, "text/plain", "Screen Already Off");
      break;
    default:
      server.send(910, "text/plain", "Invalid State");
  }
}
void actionSpeaker()
{
  int state = validateRequest(true).toInt();
  switch(state)
  {
    case ON:
      if(digitalRead(speaker) != HIGH)
      {
        Serial.println("Turning Speaker On");
        digitalWrite(speaker, HIGH);
        server.send(200, "text/plain", "Turned Speaker On");
      }
      server.send(200, "text/plain", "Speaker Already On");
      break;
    case OFF:
      if(digitalRead(speaker) != LOW)
      {
        Serial.println("Turning Speaker Off");
        digitalWrite(speaker, LOW);
        server.send(200, "text/plain", "Turned Speaker Off");
      }
      server.send(200, "text/plain", "Speaker Already Off");
      break;
    default:
      server.send(910, "text/plain", "Invalid State");
  }
}
void actionMainRouter()
{
  int state = validateRequest(true).toInt();
  switch(state)
  {
    case ON:
      if(digitalRead(mainRouter) != HIGH)
      {
        Serial.println("Turning MainRouter On");
        digitalWrite(mainRouter, HIGH);
        server.send(200, "text/plain", "Turned MainRouter On");
      }
      server.send(200, "text/plain", "MainRouter Already On");
      break;
    case OFF:
      if(digitalRead(mainRouter) != LOW)
      {
        Serial.println("Turning MainRouter Off");
        digitalWrite(mainRouter, LOW);
        server.send(200, "text/plain", "Turned MainRouter Off");
      }
      server.send(200, "text/plain", "MainRouter Already Off");
      break;
    default:
      server.send(910, "text/plain", "Invalid State");
  }
}
void actionPeripheral()
{
  int state = validateRequest(true).toInt();
  switch(state)
  {
    case ON:
      if(digitalRead(peripheral) != HIGH)
      {
        Serial.println("Turning Peripheral On");
        digitalWrite(peripheral, HIGH);
        server.send(200, "text/plain", "Turned Peripheral On");
      }
      server.send(200, "text/plain", "Peripheral Already On");
      break;
    case OFF:
      if(digitalRead(peripheral) != LOW)
      {
        Serial.println("Turning Peripheral Off");
        digitalWrite(peripheral, LOW);
        server.send(200, "text/plain", "Turned Peripheral Off");
      }
      server.send(200, "text/plain", "Peripheral Already Off");
      break;
    default:
      server.send(910, "text/plain", "Invalid State");
  }
}
// ON OFF Function's End.

void handleRoot()
{
  char html[400];
  int sec = millis()/1000;
  int min = (sec/60)%60;
  int hr = (min/60)%60;

  /* int snprintf ( char * s, size_t n, const char * format, ... );
     Composes a string with the same text that would be printed if format was used on printf, but instead of being printed,
     the content is stored as a C string in the buffer pointed by s (taking n as the maximum buffer capacity to fill).
  */
  snprintf(html, 400,
  "<html>\n\ 
    <head>\n\
      <meta http-equiv='refresh' content='5' />\n\
      <title>Home Automation</title>\n\
      <style>\n\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\n\
      </style>\n\
    </head>\n\
    <body>\n\
      <h1>Hello!!!!!</h1>\n\
      <p>Uptime: %02d:%02d:%02d</p>\n\
    </body>\n\
  </html>", hr, min, sec);

  server.send(200, "text/html", html);
}

void handleNotFound()
{
  String message = "Path Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}
