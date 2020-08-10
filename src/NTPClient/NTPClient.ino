// -----------------------------------------------------------------------------
// ---------------------------------------------------------< NTPClient.ino >---
// -----------------------------------------------------------------------------
//
// PROJECT: Arduino development for the ESP8266 custom temperature sensor board.
//
// PURPOSE: Retrieve the current time using an NTP client package.
//
// AUTHOR:  Scott Vance
//
// NOTES:   
//
// HISTORY:
// --------- ----------- - -----------------------------------------------------
// 03Mar2019 Scott Vance - Initial development.
//
// -----------------------------------------------------------------------------


#include <ESP8266WiFi.h>         // Basic wifi connection services.
#include <WiFiUdp.h>             // NTP messages are transmitted over UDP.
#include <NTPClient.h>           // NTP services.
#include <WifiConnect.h>         // Wifi network ID and credentials.



bool        WiFiConnected = false;

const int   NTPOffset = 6 * 60 * 60;            // Offset from UTC time in seconds.
const int   NTPInterval = 1 * 60 * 1000;        // Update interval In miliseconds.
const char* NTPServerName = "time.nist.gov";    // NTP server address.
os_timer_t  NTPUpdateTimer;

WiFiUDP     UDPConnection;
NTPClient   NTPClienth ( UDPConnection, NTPServerName, NTPOffset, NTPInterval );


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------< setup >---
// -----------------------------------------------------------------------------
//
// PURPOSE:     Called only once when the sketch starts, to initialize global
//              variables, pin modes, start using libraries, etc.
//
// PARAMETERS:  None
//
// RETURNS:     void
//
// NOTES:
//
// HISTORY:
// --------- ----------- -------------------------------------------------------
// 03Mar2019 Scott Vance - Initial development.
//
// -----------------------------------------------------------------------------

void setup()
{
   int         Status;
   const char* ssid      = GetWifiSSID();
   const char* password  = GetWifiPW();
   uint16_t    Tries;

   
      // Configure the onboard LED pin for output.
   pinMode ( LED_BUILTIN, OUTPUT );

   // Initialize serial output and wait for it to complete intialization.
   Serial.begin ( 115200 );
   Tries = 0;
   while ( !Serial && Tries < 25 )
   {
      Tries++;
      delay ( 100 );
   }

   // Connect to the WiFi network
   Serial.println();
   Serial.println ( "Connecting to " + ( String ) ssid );

   WiFi.mode ( WIFI_STA );
   WiFi.begin ( ssid, password );

   WiFiConnected = ( WiFi.status() == WL_CONNECTED ) ? true : false;
   for ( Tries = 0; Tries < 25 && WiFiConnected != true; Tries++ )
   {
      delay ( 500 );
      WiFiConnected = ( WiFi.status() == WL_CONNECTED ) ? true : false;
   }

   if ( WiFiConnected != true )
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


      Serial.println ( "Starting NTP client" );
      NTPClienth.begin();
      UPdateTimeStamp( NULL );
      
      os_timer_setfn ( &NTPUpdateTimer, UPdateTimeStamp, NULL );
      os_timer_arm ( &NTPUpdateTimer, NTPInterval, true );
   }
}
// ----------------------------------------------------------------< /setup >---



// -----------------------------------------------------------------------------
// ------------------------------------------------------------------< loop >---
// -----------------------------------------------------------------------------
//
// PURPOSE:     Called repeatedly to respond to inputs, update variables, and
//              generate outputs as desired.
//
// PARAMETERS:  None
//
// RETURNS:     void
//
// NOTES:
//
// HISTORY:
// --------- ----------- -------------------------------------------------------
// 03Mar2019 Scott Vance - Initial development.
//
// -----------------------------------------------------------------------------

void loop()
{
   // Do nothing.  Time stamp update driven by timer.
}

// -----------------------------------------------------------------< /loop >---



static void UPdateTimeStamp( void* )
{
   digitalWrite ( LED_BUILTIN, LOW );
   delay ( 1000 );
   digitalWrite ( LED_BUILTIN, HIGH );

   if ( WiFiConnected == true )
   {
      // Update the recorded time through the NTP client.
      NTPClienth.update();

      // Epoch: An event or a time marked by an event that begins a new period.
      //
      // The Unix epoch starts at midnight (00:00:00) of Jan 1 1970, and a Unix time
      // stamp is the number of seconds since that epoch.  
      //
      // The NTP epoch start at midnight (00:00:00) of Jan 1 1900, and a NTP time
      // stamp is the number of seconds since that epoch.  
      // 
      // The difference between the NTP and Unix epochs is defined by the Time Protocol 
      // in RFC 868 as 2208988800 seconds, so to convert from an NTP time stamp to a 
      // Unix time stamp, subtract the difference between the two from the NTP value.

      unsigned long NTPTime = NTPClienth.getEpochTime();
      
      Serial.printf ( "(UTC)  Seconds since Jan 1 1900 = %lu \n", NTPTime );

      const unsigned long NTP2Unix = 2208988800UL;

      // Subtract seventy years to get Unix time.
      unsigned long UnixTime = NTPTime - NTP2Unix;

      // Print Unix time
      Serial.printf ( "(Unix) Seconds since Jan 1 1970 = %u \n", UnixTime );

      Serial.printf ( "Time stamp formatted by the NTP client: %s \n", NTPClienth.getFormattedTime().c_str() );
      Serial.printf ( "Updating time stamp in %lu seconds... \n\n", NTPInterval / 1000 );
   }
}

