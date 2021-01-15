#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <time.h>
#include "timezone.h"

#include <WifiConnect.h>         // Wifi network ID and credentials.


os_timer_t  NTPUpdateTimer;
const int   NTPInterval = 15 * 1000;   // Update interval In miliseconds.
const char* NTPServerName1 = "pool.ntp.org"; 
const char* NTPServerName2 = "time.nist.gov";


void setup()
{
   int Status = 0;
   const char* ssid      = GetWifiSSID();
   const char* password  = GetWifiPW();
   uint16_t    Tries = 0;

   // Initialize serial output and wait for it to complete intialization.
   Serial.begin ( 115200 );
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


   bool WiFiConnected = false;

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
      
      int count = 0;
      time_t now;
      
      // void configTime ( int timezone, 
      //                   int daylightOffset_sec, 
      //                   const char* server1, 
      //                   const char* server2, 
      //                   const char* server3
      //                 );

      // Offsets for time zones are expressed in number of seconds.
      #define CST_OFFSET_SEC  ( CST * 60 *60 )

      // Daylight savings offset is either 0 or 1 hour (3600 seconds).
      #define DST_OFFSET_SEC  ( 0 )
      
      configTime ( CST_OFFSET_SEC, DST_OFFSET_SEC, NTPServerName1, NTPServerName2 );

      Serial.printf ( "Connecting to NTP server %s or %s \n", NTPServerName1, NTPServerName2 );

      //
      // It takes a while for an NTP server to connect and start returning valid values.
      // The initial return values will typically be sequential integers starting from 1.  
      // Loop until the returned values are large numbers representing a time near the
      // present time.  This can often take several minutes.
      //
      while ( Status >= 0 && now < EPOCH_1_1_2019 )
      {
         //
         // time() returns the time as the number of seconds since the 
         // Unix epoch of 1970-01-01 00:00:00 +0000 (UTC). 
         //
         Status = time ( &now );
         delay( 1000 );
         Serial.printf ( "%s", ( count % 30 == 0 ) ? "\n*" :
                               ( count % 10 == 0 ) ? " *"  :
                               "*"
                       );
         count++;
      }

      if ( Status < 0 )
      {
         Serial.println ( "ERROR: The NTP server returned failed to return a time stamp" );
      }

      else
      {
         Serial.printf ( "\nUpdate interval is %d seconds \n", NTPInterval / 1000 );
         UpdateTimeStamp( NULL );

        os_timer_setfn ( &NTPUpdateTimer, UpdateTimeStamp, NULL );
        os_timer_arm   ( &NTPUpdateTimer, NTPInterval, true );
      }
   }
}



void loop()
{
   // Do nothing
}



static void UpdateTimeStamp( void* )
{
   int Status = 0;
   time_t now;
   struct tm *timeinfo;

   //
   // An NTP connection should have already been established in setup()
   // so a non-zero result from time() would be unexpected.
   //
   Status = time ( &now );
   if ( Status < 0 )
   {
      Serial.println ( "ERROR UpdateTimeStamp: The NTP server returned failed to return a time stamp" );
   }

   else
   {
      timeinfo = localtime ( &now );
   
      Serial.printf ( "%10d = %s %02d/%02d/%d %02d:%02d:%02d \n",
                      now,
                      DAYS_OF_WEEK[ timeinfo->tm_wday ],
                      timeinfo->tm_mon + 1,
                      timeinfo->tm_mday,
                      timeinfo->tm_year + 1900,
                      timeinfo->tm_hour,
                      timeinfo->tm_min,
                      timeinfo->tm_sec
                    );                   
   }
}
