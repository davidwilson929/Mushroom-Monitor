#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <gateway.h>
#include <ESP8266WebServer.h>

/*
NOTES
------ CURRENT STATUS -------

Gateway connects to WiFi and starts access point /w updatable credentials
FLow:
Retrieve all credentials
Start AP
  - if null credentials default to open network /w SSID MUSHRROM-MONITOR
Connect to WiFi using STA credentials.
  - If null or >30sec connection time stop and opperate as AP only
Handle webserver requests
-----------------------------

Handlers to be added/updated
  - ROOT, connection stats, devices etc
  - Credentials, move to subdirectory of root, button navigation
  - Download, update to export CSV plant data, move to subdirectory of root, button navigation

Quality of Life
  - Revise all serial outputs in project to be more meaningful and modular
  - Clean gateway.h functions
  - Move HTML / web pages to seperate files to be served by LittleFS and web server

Hardware functions
  - Add hardware reset button to restore default credentials i.e. MUSHRROM-GATEWAY and no pass or STA credentials

*/
// ------- Debug Enabler --------
#define DEBUG_MODE
//-------------------------------
void serialDebug(String debugString)
{
#ifdef DEBUG_MODE
  Serial.println(debugString);
#endif
  
}
// -------Global Variables-------
bool AP_Active = false;
bool STA_Active = false;
// ------------------------------

// ------Define Web Servers------
ESP8266WebServer AP_Server(80);
// ------------------------------

void handleUpdateCredentials() //Serve form for updating WiFi STA and AP credentials
{
  AP_Server.send(200, "text/HTML","<h>Update Credentials</h>\
                                 <br>\
                                 <br>\
                                 <form action = \"/saveCredentials\" METHOD = \"post\">\
                                 <label for = \"AP_SSID\">Access Point SSID:</label>\
                                 <input type =\"text\" id=\"AP_SSID\" name=\"AP_SSID\">\
                                 <br>\
                                 <label for = \"AP_Passeord\">Access Point Password:</label>\
                                 <input type =\"text\" id=\"AP_Password\" name=\"AP_Password\">\
                                 <br>\
                                 <br>\
                                 <label for = \"STA_SSID\">Station SSID:</label>\
                                 <input type =\"text\" id =\"STA_SSID\" name =\"STA_SSID\">\
                                 <br>\
                                 <label for = \"STA_Password\">Station Password:</label>\
                                 <input type =\"text\" id =\"STA_Password\" name =\"STA_Password\">\
                                 <br>\
                                 <br>\
                                 <input type =\"submit\" value=\"Submit\">\
                                 </form>");
}

void handleSaveCredentials() //Save updated credentials for WiFi STA and AP
{
  String AP_SSID = AP_Server.arg("AP_SSID");
  String AP_Password = AP_Server.arg("AP_Password");
  String STA_SSID = AP_Server.arg("STA_SSID");
  String STA_Password = AP_Server.arg("STA_Password");

  Serial.println("STA Pass: " + STA_Password + ".");

  Serial.println("New credentials: " + AP_SSID + ", " + AP_Password + ", " + STA_SSID + ", " + STA_Password);
  updateCredentials(AP_SSID, AP_Password, STA_SSID, STA_Password);
  AP_Server.send(200, "text/plain", "saved credentials");
}

void handleConnect()
{
  AP_Server.send(200, "text/plain", "No content");
}

void handleDownload()
{
  Serial.println("File download requested");
  File download = LittleFS.open("/credentials.txt", "r");
  AP_Server.streamFile(download, "application/txt");
  download.close();
  AP_Server.sendHeader("Location","/");
  AP_Server.send(303);
}

void setup() {

//Uncomment to clean FS
//LittleFS.begin();
//LittleFS.format();
//LittleFS.end();
//
  //Start Serial for debgging
  Serial.begin(115200);
  delay(500);
  Serial.println("Serial communication started at 115200 baud"); //debug
  delay(500);

  //Attempt to fetch credentials
  String AP_SSID = getCredentials(apSSID);
  String AP_Password = getCredentials(apPassword);
  String STA_SSID = getCredentials(staSSID);
  String STA_Password = getCredentials(staPassword);

  WiFi.mode(WIFI_AP_STA); //Dual WiFi mode
  AP_Active = WiFi.softAP(AP_SSID, AP_Password); //Start Gateway AP
  if(AP_Active)
  {
    Serial.println("Sucessfully deployed AP with SSID " + AP_SSID);
    Serial.print("AP IP address is ");
    Serial.println(WiFi.softAPIP());
  }
  else
  {
    Serial.println("Failed to depoly AP");
  }

  Serial.println("");

  if (STA_SSID == "")//Check for null credentials
  {Serial.println("No STA credentials stored, not attempting to connect to WiFi");}
  else if(STA_Password == "")
  {Serial.println("No STA credentials stored, not attempting to connect to WiFi");}
  else
  {
      STA_Active = WiFi.begin(STA_SSID, STA_Password); //Start Gateway STA
      Serial.print("Connecting");    
      for (int8 i = 0; i < 30 && WiFi.status() != WL_CONNECTED; i++)
      {
        delay(1000);
        Serial.print(" .");
      }

      if(WiFi.status() == WL_CONNECTED)
      {
        Serial.println("");
        Serial.println("Succesfully connected to " + STA_SSID);
      }
      else
      {
        Serial.println("Failed to connect to WiFi as station, aboring process");
      }
  }
  //Handlers for AP server
  AP_Server.on("/", handleConnect);
  AP_Server.on("/credentials", handleUpdateCredentials);
  AP_Server.on("/saveCredentials", handleSaveCredentials);
  AP_Server.on("/download", handleDownload);
  if(AP_Active){AP_Server.begin();}

  //Handlers for STA server
}

void loop() {
  if (AP_Active)
  {
  AP_Server.handleClient();
  }
}