//
// Modified version of web server login code from:
// https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html
//

#include <Esp.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266WebServer.h>


// Create a wifi instance that will connect to next available 
// access point if current connection is lost.
ESP8266WiFiMulti wifiMulti;     

// Create a webserver on port 80
ESP8266WebServer WebServer ( 80 );    


void handleRoot();
void handleLogin();
void handleNotFound();


void setup ( void )
{
   char host_name[ 16 ] = { 0 };
   uint32_t ChipID = ESP.getChipId();

   
   sprintf ( host_name, "%s-%02x%02x%02x",
             "esp8266",
             ( uint16_t ) ( ( ChipID >> 16 ) & 0xff ),
             ( uint16_t ) ( ( ChipID >>  8 ) & 0xff ),
             ( uint16_t ) ChipID & 0xff
           );

   Serial.begin ( 115200 );
   delay ( 10 );
   Serial.println('\n');
   
   wifiMulti.addAP ( "OurHouse24",   "USS Cape Cod AD-43" );
   wifiMulti.addAP ( "OurYard18-24", "USS Code Cod AD-43" );
   wifiMulti.addAP ( "Castle",       "USS Cape Cod AD-43" );
   WiFi.hostname ( host_name );

   Serial.printf ( "Connecting ..." );


   // Scan specified wiFi networks, and connect to the strongest one.
   while ( wifiMulti.run() != WL_CONNECTED ) 
   { 
     delay ( 250 );
     Serial.print ( '.' );
   }
   Serial.printf ( "\n" );

   Serial.print ( "Connected to " );
   Serial.println ( WiFi.SSID() );
   Serial.print ( "IP address: " );
   Serial.println ( WiFi.localIP() );
   Serial.printf ( "Host name: %s \n", host_name ); 
   
   WebServer.on ( "/", HTTP_GET, handleRoot );        
   WebServer.on ( "/login", HTTP_POST, handleLogin); 
   WebServer.onNotFound ( handleNotFound );           
   
   WebServer.begin();
   Serial.println ( "HTTP server started" );
}  

   
void loop ( void )
{
   WebServer.handleClient();
}  
   
void handleRoot() 
{
   char html[] =  "<html> \n"
                  "<head> \n"
                  "<title>Login Example</title> \n"
                  "</head> \n"
                  "<body> \n"
                  "<center> \n"
                  "<h1>Login</h1> <hr> \n"
                  "</center> \n"
                  "<form action=\"/login\" method=\"POST\"> \n"
                  "<input type=\"text\" name=\"username\" placeholder=\"Username\"></br> \n"
                  "<input type=\"password\" name=\"password\" placeholder=\"Password\"></br> \n"
                  "<input type=\"submit\" value=\"Login\"> \n"
                  "</form> \n"
                  "<p>Try 'John Doe' and 'password123' ...</p> \n"
                  "</body> \n"
                  "</html> \n"
                  ;

   // When URI / is requested, send a web page with a button to toggle the LED
   WebServer.send ( 200, "text/html", html );
}  
   
void handleLogin() 
{
   if (  ! WebServer.hasArg ( "username" ) 
      || ! WebServer.hasArg ( "password" ) 
      || WebServer.arg ( "username" ) == NULL 
      || WebServer.arg ( "password" ) == NULL
      ) 
   { 
      // The request is invalid, so send HTTP status 400
      WebServer.send ( 400, "text/plain", "400: Invalid Request" );
      return;
   }

   // NOTE: This is obviously not a secure storage or comparison technique.  The thing to do
   // is to record the user name and password in a hashed format, either in a SPIFFS file, or
   // in ROM, and then hash the input values and compare hash-to-hash for authorization.
   
   if (  WebServer.arg ( "username" ) == "John Doe" 
      && WebServer.arg ( "password" ) == "password123"
      ) 
   { 
      WebServer.send ( 200, "text/html", "<h1>Welcome, " + WebServer.arg ( "username" ) + "!</h1><p>Login successful</p>" );
   } 

   else 
   {
      WebServer.send ( 401, "text/plain", "401: Unauthorized" );
   }
}


void handleNotFound()
{
   WebServer.send ( 404, "text/plain", "404: Not found" );
}
