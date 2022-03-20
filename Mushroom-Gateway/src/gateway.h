//Gateway specific header file

#include <Arduino.h>
#include <LittleFS.h>

//-------- Debug Enabler --------
// Uncomment to send debug messages over serial

//#define DEBUG_MODE

//-------------------------------

//----- File System CLeaner -----
// Uncomment to format file system on reset/power on

//#define CLEAN_FS

//-------------------------------

void serialDebug(String debugString)
{
    #ifdef DEBUG_MODE
    Serial.begin(115200);
    Serial.println(debugString);
    Serial.end();
    delay(1000); //delay by 1 second so I have at least some idea of when shit is happening
    #endif
}

void formatFS()
{
    #ifdef CLEAN_FS
        serialDebug("Attempting to format file sytem . . .");
        switch (LittleFS.format())
        {
        case true:
            serialDebug("File system sucesfully formatted!");
            break;
        
        case false:
            serialDebug("File system formatting failed. No idea why, guess I can figure it out later if it ever happens?");
            break;
        }
    #endif
}


typedef enum getCredentials
{
    apSSID = 0, apPassword = 1, staSSID = 2, staPassword = 3
}getCredentials_t;


//Retrieve stored WiFi credentials from onboard flash
String getCredentials(int8 credentialsRequired)
{
    serialDebug("Attempting to retireve stored credentials  . . .");
    bool success = LittleFS.begin(); //Attempt to mount file system
    if (!success)
    {
        return "";
        serialDebug("Failed to mount file system, unable to retrieve credentials");
    }

    success = LittleFS.exists("/credentials.txt"); //Check if credentiials file exists
    if (!success) //If no file return default AP SSID with no password and no STA credentials
    {
        serialDebug("credentials.txt does not exist, returning default credentials");
        switch (credentialsRequired)
        {
        case 0:
            return "Mushroom-Gateway";
            break;
        default:
            return "";
            }
    }
    serialDebug("credentials.txt exists! Parsing file . . .");
    File credentials = LittleFS.open("/credentials.txt", "r");//Open credentials file in ead only mode
    String returnString;
    switch (credentialsRequired)//parse file and return resulting string depending on credential requested
    {
        case apSSID:
            serialDebug("Parsing file for AP SSID . . .");
            for (int8 i = 0; i < credentialsRequired; i++)
            {
                credentials.readStringUntil(',');
            }

            returnString = credentials.readStringUntil(',');

            if(returnString == "")//Handle null
            {
                returnString = "Mushroom-Monitor";
            }
            credentials.close();
            break;

        case apPassword:
            serialDebug("Parsing file for AP password . . .");

            for (int8 i = 0; i < credentialsRequired; i++)
            {
                credentials.readStringUntil(',');
            }

            returnString = credentials.readStringUntil(',');
            credentials.close();
            break;

        case staSSID:
            serialDebug("Parsing file for STA SSID . . .");

            for (int8 i = 0; i < credentialsRequired; i++)
            {
                credentials.readStringUntil(',');
            }

            returnString = credentials.readStringUntil(',');
            credentials.close();
            break;

        case staPassword:
            serialDebug("Parsing file for STA password . . .");

            for (int8 i = 0; i < credentialsRequired; i++)
            {
                credentials.readStringUntil(',');
            }

            returnString = credentials.readStringUntil(',');
            credentials.close();
            break;

        default:
            returnString = "";
        }

    serialDebug("Returned credential: " + returnString);
    return returnString;
}


    //update WiFi credentials while in AP mode
    bool updateCredentials(String apSSID, String apPassword, String staSSID, String staPassword)
    {   
        serialDebug("Attempting to update credentials . . .");
        if (LittleFS.exists("/credentials.txt")) //Check for existing credentials file and copy data if null entered
        {
            if (apSSID == "")
            {
                apSSID = getCredentials(0);   
            }
            if (apPassword == "")
            {
                apPassword = getCredentials(1);
            }
            if (staSSID == "")
            {
                staSSID = getCredentials(2);
            }
            if (staPassword == "")
            {
                staPassword = getCredentials(3);
            }
    }
    //Save new credentials to file3
    serialDebug("Opening credentials.txt in write mode . . .");
    File credentials = LittleFS.open("/credentials.txt", "w+");
    String strCredentials;
    credentials.print(apSSID + ",");
    credentials.print(apPassword  + ",");
    credentials.print(staSSID  + ",");
    credentials.print(staPassword + ",");
    credentials.seek(0, SeekCur);
    strCredentials = credentials.readString();
    serialDebug("credentials.txt updated to: " + strCredentials);
    credentials.close();

    return true;
}