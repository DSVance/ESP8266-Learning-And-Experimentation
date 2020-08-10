// -----------------------------------------------------------------------------
// ------------------------------------------------------------< MyBasicOTA >---
// -----------------------------------------------------------------------------
//
// PURPOSE:    Experiement with Over-The-Air (OTA) updating of the firmware on
//             the ESP8266 module.
//
// NOTES:      -  With OTA the Serial Monitor in the Audrino IDE will not work
//                because the computer is not actually communicating to the 
//                ESP8266 over the serial port - its using wifi to communicaite.
//
//                To see what is being written to the serial port an external 
//                serial monitor (like Termite) is needed.  Connect the ESP8266
//                to the computer's serial port and select that port and the 
//                proper baud rate in the monitor program.  The port may or may
//                not be different that the one used in the Audrino IDE, but the
//                baud rate should be the same since that is what the ESP8266 
//                was programmed to use.  Common serial settings are:
//                
//                   Port ..... COM9   Data bits ... 8   Parity ......... None
//                   Baud ... 115200   Stop bits ... 1   Flow Control ... None
//
//                If an OTA update fails, using an external serial monitor will
//                likely provide an indication by showing errors caught by the 
//                uploader, exceptions and possibly a stack trace.
//
// HISTORY:
// --------- ------------ -----------------------------------------------------
// 13Oct2018 Scott Vance  Initial development.
//
// ----------------------------------------------------------------------------- 



#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Common.h>
#include <WifiConnect.h>



// -----------------------------------------------------------------------------
// -----------------------------------------------------------------< setup >---
// -----------------------------------------------------------------------------
//
// PURPOSE:    Called only once when the sketch starts, to initialize global 
//             variables, pin modes, start using libraries, etc.
//
// PARAMETERS: None
//
// RETURNS:    void
//
// NOTES:
//
// HISTORY:
// --------- ------------ -----------------------------------------------------
// 13Oct2018 Scott Vance  Initial development.
//
// ----------------------------------------------------------------------------- 

void setup() 
{
   const char* ssid      = GetWifiSSID();
   const char* password  = GetWifiPW();
   uint16_t    Tries;
   bool        WiFiConnected = false;

   Serial.begin ( 115200 );
   delay ( 100 );

   // Connect to the WiFi network
   Serial.println ();
   Serial.println ( "Connecting to " + (String) ssid );

   // Set the Wifi to station mode (STA) versus access point (AP).
   WiFi.mode ( WIFI_STA );
   WiFi.begin ( ssid, password );

   WiFiConnected = ( WiFi.status() == WL_CONNECTED ) ? true : false;
   for ( Tries = 0; Tries < 25 && WiFiConnected != true; Tries++ )
   {
      delay ( 500 );
      WiFiConnected = ( WiFi.status() == WL_CONNECTED ) ? true : false;
   }

   if ( WiFiConnected == false )
   {
      Serial.println ( "ERROR: Wifi failed to start" );
   }

   else
   {
      Serial.print   ( "WiFi connected to: " );
      Serial.println ( WiFi.localIP() );
      Serial.print   ( "Netmask: " );
      Serial.println ( WiFi.subnetMask() );
      Serial.print   ( "Gateway: " );
      Serial.println ( WiFi.gatewayIP() );
      Serial.println ();
      
      // Port defaults to 8266
      // ArduinoOTA.setPort(8266);

      // Hostname defaults to esp8266-[ChipID]
      // ArduinoOTA.setHostname("myesp8266");

      // No authentication by default
      // ArduinoOTA.setPassword("admin");

      // Password can be set with it's md5 value as well
      // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
      // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

      ArduinoOTA.onStart([]() 
      {
         String type;
        
         if ( ArduinoOTA.getCommand() == U_FLASH ) 
         {
            type = "sketch";
         } 
    
         else 
         { 
            // U_SPIFFS
            type = "filesystem";
         }

         // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
         Serial.println ( "Start updating " + type );
      });
  
      ArduinoOTA.onEnd([]() 
      {
         Serial.println ( "\nEnd" );
      });
  
      ArduinoOTA.onProgress( []( unsigned int progress, unsigned int total ) 
      {
         Serial.printf ( "Progress: %u%%\r", (progress / (total / 100)) );
      });
  
      ArduinoOTA.onError( []( ota_error_t error )
      {
         Serial.printf ( "Error[%u]: ", error );
         if      ( error == OTA_AUTH_ERROR )    { Serial.println ( "Auth Failed" );    } 
         else if ( error == OTA_BEGIN_ERROR )   { Serial.println ( "Begin Failed" );   } 
         else if ( error == OTA_CONNECT_ERROR ) { Serial.println ( "Connect Failed" ); } 
         else if ( error == OTA_RECEIVE_ERROR ) { Serial.println ( "Receive Failed" ); } 
         else if ( error == OTA_END_ERROR )     { Serial.println ( "End Failed" );     }
      });
  
      ArduinoOTA.begin();

      Serial.println ( "Ready" );
   }
}

// ----------------------------------------------------------------< /setup >---



// -----------------------------------------------------------------------------
// ------------------------------------------------------------------< loop >---
// -----------------------------------------------------------------------------
//
// PURPOSE:    Called repeatedly to respond to inputs, update variables, and 
//             generate outputs as desired.
//
// PARAMETERS: None
//
// RETURNS:    void
//
// NOTES:
//
// HISTORY:
// --------- ------------ -----------------------------------------------------
// 13Oct2018 Scott Vance  Initial development.
//
// -----------------------------------------------------------------------------

void loop() 
{
   ArduinoOTA.handle();
}

// -----------------------------------------------------------------< /loop >---
