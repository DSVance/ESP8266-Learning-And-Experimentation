// ----------------------------------------------------------------------------
// This source file is running a simple websocket server, which in conjunction
// with a simple web application (web page and javascript) on the Enterprise
// server (see \\Enterprise\Web\Experiments\TemperatureData.html), allows the
// NodeMCU system to push data updates directly to the web page.
//
// This routine uses a timer that fires at a set interval and invokes a
// specified function when it fires.  That function transmits the temperature
// data to the connected clients (and it this case generates the data also).
//
// Nothing happens in the main loop other than letting the web socket server
// service its clients.
// ----------------------------------------------------------------------------



#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WebSocketsServer.h>
#include <WifiConnect.h>



// Instantiate a web socket server on port 81.
WebSocketsServer  WebSocket = WebSocketsServer ( 81 );
os_timer_t        TemperatureTimer;



void SensorAction ( void* Args )
{
   float SensorValueF;
   char  SensorValueString[16];
   int   SensorValueStringLength;

   // Little flash to let us know you're working.
   digitalWrite ( LED_BUILTIN, LOW );
   delay ( 300 );

   // Simulate data with random numbers in abscense of a sensor.
   SensorValueF = (float) random ( -2099, 11099 );
   SensorValueF /= 100;
   SensorValueStringLength = sprintf ( SensorValueString, "%-3.1f  °F", SensorValueF );
   Serial.printf ( "Send: %s \n", SensorValueString );

   // Transmit the data to any connected web-socket clients.
   WebSocket.broadcastTXT ( SensorValueString, SensorValueStringLength );
   digitalWrite ( LED_BUILTIN, HIGH );
}



void WebSocketEvent (
   uint8_t   num,                // ID number of the socket
   WStype_t  type,               // Event type
   uint8_t*  payload,            // Message associated with the event
   size_t    payload_length      // Length of the associated message
)
{
   Serial.print ( "Web Socket Event - " );
   switch ( type )
   {
      case WStype_ERROR:
      {
         Serial.printf ( "ERROR [%u]: %s \n",
                         num,
                         ( payload != NULL ) ? payload : (unsigned char*) " "
                       );
         break;
      }

      case WStype_DISCONNECTED:
      {
         Serial.printf ( "DISCONNECTED [%u] \n", num );
         break;
      }

      case WStype_CONNECTED:
      {
         // NOTE: The payload argument carries the URL requested.
         IPAddress ip = WebSocket.remoteIP ( num );
         Serial.printf ( "CONNECTED [%u] from %d.%d.%d.%d  url: %s \n",
                         num,
                         ip[0],
                         ip[1],
                         ip[2],
                         ip[3],
                         payload
                       );
         break;
      }

      case WStype_TEXT:
      {
         Serial.printf ( "TEXT [%u] %s \n",
                         num,
                         ( payload != NULL ) ? payload : (unsigned char*) " "
                       );
         break;
      }

      default:
      {
         // WARNING: Should never happen!
         Serial.printf ( "UNKNOWN [%u] %s \n",
                         num,
                         ( payload != NULL ) ? payload : (unsigned char*) " "
                       );
         break;
      }
   }
}



void setup()
{
   const char* ssid      = GetWifiSSID();
   const char* password  = GetWifiPW();

   // Blink the onboard LED pin to indicate START.
   pinMode ( LED_BUILTIN, OUTPUT );
   for ( int i = 0; i < 5; i++ )
   {
      digitalWrite ( LED_BUILTIN, LOW );
      delay ( 250 );
      digitalWrite ( LED_BUILTIN, HIGH );
      delay ( 250 );
   }


   Serial.begin ( 115200 );
   while ( !Serial )
   {
      delay ( 100 );
   }

   // Connect to the WiFi network
   Serial.printf ( "Connecting to %s \n", ssid );

   WiFi.begin ( ssid, password );

   while ( WiFi.status() != WL_CONNECTED )
   {
      delay ( 100 );
      Serial.print ( "." );
   }

   Serial.println ( "" );
   Serial.println ( "WiFi connected" );
   Serial.print   ( "IP address: " );
   Serial.println ( WiFi.localIP() );
   Serial.print   ( "Netmask: " );
   Serial.println ( WiFi.subnetMask() );
   Serial.print   ( "Gateway: " );
   Serial.println ( WiFi.gatewayIP() );

   WebSocket.onEvent ( WebSocketEvent );
   WebSocket.begin();

   Serial.println ( "Web socket server started!" );

   // Prime the RNG with garbage value from the analog pin.
   randomSeed ( analogRead ( 0 ) );


   // Enable the timer that transmits data to web socket client.
   os_timer_setfn ( &TemperatureTimer, SensorAction, NULL );
   os_timer_arm ( &TemperatureTimer, 3500, true );
}


void loop()
{
   WebSocket.loop();
}

