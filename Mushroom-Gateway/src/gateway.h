//Gateway specific header file

#include <Arduino.h>
#include <LittleFS.h>

typedef enum getCredentials
{
    apSSID = 0, apPassword = 1, staSSID = 2, staPassword = 3
}getCredentials_t;


//Retrieve stored WiFi credentials from onboard flash
String getCredentials(int8 credentialsRequired)
{
    bool success = LittleFS.begin(); //Mount FS and return error if fails
    if (!success){return "Failed to mount filesystem";}

    success = LittleFS.exists("/credentials.txt"); //Check if credentiials file exists
    if (!success) //If no file return defaults
    {
        Serial.println("txt file dows not exist");
        switch (credentialsRequired)
        {
        case 0:
            return "Mushroom-Gateway";
            break;
        default:
            return "";
            }
    }
    Serial.println("File Exists");
    File credentials = LittleFS.open("/credentials.txt", "r");
    String returnString;
    switch (credentialsRequired)
    {
    case apSSID:
    Serial.println("Case APSSID"); //debug
        for (int8 i = 0; i < credentialsRequired; i++)
        {
            credentials.readStringUntil(',');
        }
        returnString = credentials.readStringUntil(',');
        if(returnString == ""){returnString = "Mushroom-Monitor";}//Handle null
        credentials.close();
        break;
    case apPassword:
        Serial.println("Case APPASS"); //debug

        for (int8 i = 0; i < credentialsRequired; i++)
        {
            credentials.readStringUntil(',');
        }
        returnString = credentials.readStringUntil(',');
        credentials.close();
        break;
    case staSSID:
        Serial.println("Case STASSID");//debug

        for (int8 i = 0; i < credentialsRequired; i++)
        {
            credentials.readStringUntil(',');
        }
        returnString = credentials.readStringUntil(',');
        credentials.close();
        break;

    case staPassword:
        Serial.println("Case STAPASS"); //debug

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

    Serial.println("Returned Value: " + returnString);
    return returnString;
}
//update WiFi credentials while in AP mode
bool updateCredentials(String apSSID, String apPassword, String staSSID, String staPassword)
{   
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
    //Save new credentials to file
    File credentials = LittleFS.open("/credentials.txt", "w");
    credentials.print(apSSID + ",");
    credentials.print(apPassword  + ",");
    credentials.print(staSSID  + ",");
    credentials.print(staPassword + ",");
    credentials.close();

    return true;
}