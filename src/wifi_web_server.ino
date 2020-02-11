// -----------------------------------------------------------------------------
// ---------------------------------------------------< wifi_web_server.ino >---
// -----------------------------------------------------------------------------
//
// PURPOSE: Experiment with establishing a wifi connection and web server.
//
// MACHINE: ESP8266
//
// NOTES:   -  Connects to the wifi network specified at the beginning of the
//             'setup' function.  If unable to connect to that network, the wifi
//             is started in access-point mode with a hard coded SSID and a hard
//             coded IP address.  
//
//          -  If the wifi is started in AP mode, a wifi device like a cell
//             phone can connect using the SSID defined in the AP section of the
//             code (something like ESP###### where the numbers are the chip ID
//             of the ESP8266 device.
//
//          -  Once the wifi is up (in either station mode or AP mode), a web
//             page is surfaced at the IP address assigned to the ESP8266.
//
//          -  Staring in AP mode can be forced by using the invalid network
//             credentials below.  Use the proper ones to connect normally.
//
// HISTORY:
// --------- --- - -------------------------------------------------------------
// 24Aug2018 DSV - Initial development.
// 28Nov2018 DSV - Added starting in AP mode if initial connection fails.
//
// -----------------------------------------------------------------------------



#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WifiConnect.h>

 
String   page      = "";
bool     WiFiConnected = false;
bool     WiFiAPMode = false;


//Instantiate a web server on port 80.
ESP8266WebServer WebServer( 80 );


void setup() 
{
#if 0
  const char* ssid      = GetWifiSSID();
  const char* password  = GetWifiPW();
#endif

  const char* ssid      = "OurHouse24xxx";
  const char* password  = "BadFood";

   uint16_t    Tries;

  
   // Configure the onboard LED pin for output.
   pinMode ( LED_BUILTIN, OUTPUT );
   // Turn the LED off.
   digitalWrite ( LED_BUILTIN, HIGH );

   Serial.begin ( 115200 );

   Tries = 0;
   while ( !Serial && Tries < 25 )
   {
      Tries++;
      delay ( 100 );
   }

   // Connect to the WiFi network
   Serial.println();
   Serial.print ( "Connecting to " );
   Serial.println ( ssid );


   WiFi.mode ( WIFI_STA );
   WiFi.begin ( ssid, password );
  
   WiFiConnected = ( WiFi.status() == WL_CONNECTED ) ? true : false;
   for ( Tries = 0; Tries < 25 && WiFiConnected != true; Tries++ )
   {
      delay ( 500 );
      WiFiConnected = ( WiFi.status() == WL_CONNECTED ) ? true : false;
   }
 
   if ( WiFiConnected == true )
   {
      Serial.print   ( "WiFi connected to: " );
      Serial.println ( WiFi.localIP() );
      Serial.print   ( "Netmask: " );
      Serial.println ( WiFi.subnetMask() );
      Serial.print   ( "Gateway: " );
      Serial.println ( WiFi.gatewayIP() );
      Serial.println ();

      delay ( 2000 );
   }

   else
   {
      IPAddress   LocalIP ( 192, 168, 9, 11 );
      IPAddress   Gateway ( 192, 168, 0, 62 );
      IPAddress   Subnet ( 255, 255, 255, 0 );
      String      APName = "ESP" + String ( ESP.getChipId() );
         
      Serial.println ( "ERROR: Wifi failed to start" );
      Serial.println ( "Starting wifi soft access point \"" + APName + "\"" );

      WiFi.softAPConfig ( LocalIP, Gateway, Subnet );

      WiFiAPMode = WiFi.softAP ( APName.c_str() );
      if ( WiFiAPMode == true )
      {
         Serial.print   ( "Access point IP address: ");
         Serial.println ( WiFi.softAPIP() );
         Serial.println ( "Wifi AP mode ready for connections" );
      }

      else
      {
         Serial.println ( "Wifi AP mode failed to start!" );
      }
   }

  
   //The HTML of the web page.
   page = "<html> \n"  \
          "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /></head> \n" \
          "<h1>ESP8266 Home</h1> \n" \
          "<p> \n" \
          "<a href=\"LEDOn\"><button>ON</button></a>&nbsp; \n" \
          "<a href=\"LEDOff\"><button>OFF</button></a> \n" \
          "</p> \n" \
          "</html> \n"
          ;

   WebServer.on ( "/", []()
   {
      WebServer.send ( 200, "text/html", page );
   });
  
   WebServer.on ( "/LEDOn", []()
   {
      WebServer.send ( 200, "text/html", page );
      digitalWrite ( LED_BUILTIN, LOW );
   });
  
   WebServer.on ( "/LEDOff", []()
   {
      WebServer.send ( 200, "text/html", page );
      digitalWrite ( LED_BUILTIN, HIGH );
   });
  
   WebServer.begin();
   Serial.println ( "Web server started!" );
}
 
 
void loop() 
{
   static boolean ErrorShown = false;
   static int8_t  LastConnectionCount = -1;

   
   if ( WiFiConnected == true || WiFiAPMode == true )
   {
      int8_t CurrentConnectionCount = WiFi.softAPgetStationNum();
      
      if ( LastConnectionCount != CurrentConnectionCount )
      {
         LastConnectionCount = CurrentConnectionCount;
         Serial.printf ( "Stations connected = %d \n", CurrentConnectionCount );
      }
      WebServer.handleClient();
   }

   else if ( ErrorShown == false )
   {
      Serial.println ( "No wifi available for connections" );
      ErrorShown = true;
   }
}
