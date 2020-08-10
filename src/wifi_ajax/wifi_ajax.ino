
// NOTE: This works, but with a couple of issues:
//    1) The web page only shows every other data value.
//    2) The data value is on its own line rather than inline.

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WifiConnect.h>

//Instantiate a web server on port 80.
ESP8266WebServer WebServer( 80 );

String page = "";
String text = "";
int    count = 0;
double data;

void setup() 
{
  const char* ssid      = GetWifiSSID();
  const char* password  = GetWifiPW();

  // Configure the analog pin for input.
  // pinMode ( A0, INPUT );
  //
  // In the abscense of a sensor, generate random data.  Seed the
  // random number generator with noise from input pin 0.
  randomSeed ( analogRead ( 0 ) );

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
  page = "<html> \n" \
         "<head></head> \n" \
         "<h1>Sensor to NodeMCU Web Server</h1> \n" \
         "<br> \n" \
         "<b>Data:</b> <div id=\"data\"></div> \n" \
         "<script> \n" \
         "var x = setInterval ( function(){ loadData ( \"data.txt\", updateData ) }, 1000 ); \n" \
         "function loadData ( url, callback ) \n" \
         "{ \n" \
         "   var xhttp = new XMLHttpRequest(); \n" \
         "   xhttp.onreadystatechange = function() \n" \
         "   { \n" \
         "      if( this.readyState == 4 && this.status == 200 ) \n" \
         "      { \n" \
         "         callback.apply ( xhttp ); \n" \
         "      } \n" \
         "   }; \n" \
         "   xhttp.open ( \"GET\", url, true ); \n" \
         "   xhttp.send(); \n" \
         "} \n" \
         " \n" \
         "function updateData() \n" \
         "{ \n" \
         "   document.getElementById ( \"data\" ).innerHTML = this.responseText; \n" \
         "}\n" \
         "</script> \n" \
         "</html> \n"
         ;

  WebServer.on ( "/", [](){
    WebServer.send ( 200, "text/html", page );
  });

  WebServer.on ( "/data.txt", [](){
    text = (String) data;
    WebServer.send ( 200, "text/html", text );
  });
  
  WebServer.begin();
  Serial.println ( "Web server started!" );
}
 
 
void loop() 
{
  // Retrieve data from the analog sensor pin.
  // data = analogRead(A0);
  //
  //Simulate data with random numbers in abscense of a sensor.
  data = (double) random ( 1, 10000 );
  data /= 100;

  count++;
  Serial.print ( count );
  Serial.print ( " Data: " );
  Serial.println ( data );

  WebServer.handleClient();

  // Turn the onboard LED on.
  digitalWrite ( LED_BUILTIN, LOW );
  delay ( 250 );

  // Turn the onboard LED off.
  digitalWrite ( LED_BUILTIN, HIGH );
  delay ( 3000 );
}

