#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <WifiConnect.h>


WebSocketsServer WebSocket = WebSocketsServer ( 81 );
 
//Instantiate a web server on port 80.
ESP8266WebServer WebServer( 80 );

String page      = "";


void setup() 
{
  const char* ssid      = GetWifiSSID();
  const char* password  = GetWifiPW();


  // Configure the onboard LED pin for output.
  pinMode ( LED_BUILTIN, OUTPUT );
  // Turn the LED off.
  digitalWrite ( LED_BUILTIN, HIGH );
  
  Serial.begin ( 115200 );
  delay ( 100 );

  // Connect to the WiFi network
  Serial.println();
  Serial.print ( "Connecting to " );
  Serial.println ( ssid );
  
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

  //The HTML of the web page.
  page = "<html> \n"  \
         "<head></head> \n" \
         "<body> \n" \
         "<h1>Simple NodeMCU WebSocket Server</h1> \n" \
         "<p> \n" \
         "<a href=\"LEDOn\"><button>ON</button></a>&nbsp; \n" \
         "<a href=\"LEDOff\"><button>OFF</button></a> \n" \
         "</p> \n" \
         "</body> \n" \
         "</html> \n"
          ;

  WebServer.on ( "/", [](){
    WebServer.send ( 200, "text/html", page );
  });
  
  WebServer.on ( "/LEDOn", [](){
    WebServer.send ( 200, "text/html", page );
    digitalWrite ( LED_BUILTIN, LOW );
    delay ( 1000 );
  });
  
  WebServer.on ( "/LEDOff", [](){
    WebServer.send ( 200, "text/html", page );
    digitalWrite ( LED_BUILTIN, HIGH );
    delay ( 1000 ); 
  });
  
  WebServer.begin();
  WebSocket.begin();
  WebSocket.onEvent ( WebSocketEvent );
  Serial.println ( "Web server started!" );
}
 
 
void loop() 
{
  WebSocket.loop();
  WebServer.handleClient();
  if ( Serial.available() > 0 )
  {
    char c[] = { (char) Serial.read() };
    WebSocket.broadcastTXT ( c, sizeof(c) );
  } 
}


void WebSocketEvent ( 
  uint8_t   num, 
  WStype_t  type, 
  uint8_t*  payload, 
  size_t    length
)
{
  if ( type == WStype_TEXT )
  {
    for ( int i = 0; i < length; i++) 
    {
      Serial.print( (char) payload[i] );
    }
    
    Serial.println();
  }
}

