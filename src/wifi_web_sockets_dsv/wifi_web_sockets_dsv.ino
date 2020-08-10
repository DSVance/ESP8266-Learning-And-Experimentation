// ----------------------------------------------------------------------------
// This modified version of wifi_web_sockets.ino has been changed to send
// a reply on the web socket when a text message is recieved.
//
// This source file is running a simple websocket server, in conjunction with 
// a simple web application (web page and javascript) on the Enterprise 
// server (see \\Enterprise\Web\Experiments\WebSocket.html) allowed passing
// of messages back and forth between the application and the websocket server.
// See the serial output window for recieved messages.  Reply messages are
// sent automatically when a new text message is recieved.
// ----------------------------------------------------------------------------

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <WifiConnect.h>


WebSocketsServer WebSocket = WebSocketsServer ( 81 );


void setup() 
{
   const char* ssid      = GetWifiSSID();
   const char* password  = GetWifiPW();
  
   Serial.begin ( 115200 );
   while ( !Serial )
   {
      delay ( 100 );
   }

   // Connect to the WiFi network
   Serial.printf ( "Connecting to %s", ssid );
  
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
 
   WebSocket.begin();
   WebSocket.onEvent ( WebSocketEvent );

   Serial.println ( "Web socket initialized!" );
}
 
 
void loop() 
{
   WebSocket.loop();

   if ( Serial.available() > 0 )
   {
      char c[] = { (char) Serial.read() };
      WebSocket.broadcastTXT ( c, sizeof(c) );
   } 
}


void LogWebSocketEvent (
   WStype_t  type, 
   uint8_t*  payload, 
   size_t    payload_length
)
{
   Serial.print ( "Web Socket Event - " );
   switch ( type )
   {
      case WStype_ERROR:        Serial.print ( "ERROR: " );                                 break;
      case WStype_DISCONNECTED: Serial.print ( "DISCONNECT" );                              break;
      case WStype_CONNECTED:    Serial.print ( "CONNECT: " );                               break;
      case WStype_TEXT:         Serial.print ( "TEXT(" + (String)payload_length + "): " );  break;
      default:                  Serial.print ( "UNKNOWN: " );                               break;
   }

   for ( int i = 0; i < payload_length; i++) 
   {
      Serial.print( (char) payload[i] );
   }
   Serial.println();  
}



void WebSocketEvent ( 
   uint8_t   num, 
   WStype_t  type, 
   uint8_t*  payload, 
   size_t    payload_length
)
{
   char     reply_msg[24];
   size_t   reply_len;

   LogWebSocketEvent ( type, payload, payload_length );

   if ( type == WStype_TEXT )
   {
      //
      // Send a reply to the message on the websocket.
      //
      reply_len = sprintf ( reply_msg, "%s %d \n", "Received length", payload_length );
      Serial.print ( "Sending " + (String)reply_len + " characters: " + reply_msg );
  
      // TODO: The broadcast works, but it would be better to send to a specific target.  How?
      WebSocket.broadcastTXT ( reply_msg, reply_len );    
   }
}

