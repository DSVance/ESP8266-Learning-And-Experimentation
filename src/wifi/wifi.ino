#include <ESP8266WiFi.h>
#include <WifiConnect.h>


// Connects to the specified wifi SSID, or if unable to connnect, starts the 
// wifi in access-point mode.  A wifi device cn then connect to the ESP8266
// using the hard coded SSID defined in the AP section.


bool WiFiConnected = false;
bool WiFiAPMode = false;



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
// --------- ------ ------------------------------------------------------------
// 13Oct2018 DSV    Initial development.
//
// -----------------------------------------------------------------------------

void setup()
{
  const char* ssid      = GetWifiSSID();
  const char* password  = GetWifiPW();
#if 0
  const char* ssid      = "OurHouse24xxx";
  const char* password  = "USS Cape Cod AD-43";
#endif
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

  ScanWifi();

  // Connect to the WiFi network
  Serial.println();
  Serial.println ( "Connecting to " + (String) ssid );

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
    IPAddress LocalIP ( 192, 168, 9, 11 );
    IPAddress Gateway ( 192, 168, 0, 62 );
    IPAddress Subnet ( 255, 255, 255, 0 );

    Serial.println ( "ERROR: Wifi failed to start" );
    Serial.println ( "Starting wifi soft access point" );

    WiFi.softAPConfig ( LocalIP, Gateway, Subnet );

    WiFiAPMode = WiFi.softAP ( "ESPsoftAP_01" );
    if ( WiFiAPMode == true )
    {
      Serial.print   ( "Soft-AP IP address: ");
      Serial.println ( WiFi.softAPIP() );
      Serial.println ( "Wifi AP mode ready for connections" );
    }

    else
    {
      Serial.println ( "Wifi AP mode failed to start!" );
    }
  }
}

// ----------------------------------------------------------------< /setup >---


int value = 0;


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
// --------- ------ ------------------------------------------------------------
// 13Oct2018 DSV    Initial development.
//
// -----------------------------------------------------------------------------

void loop()
{
  delay ( 5000 );
  value++;

  digitalWrite ( LED_BUILTIN, LOW );
  delay ( 1000 );
  digitalWrite ( LED_BUILTIN, HIGH );

  if ( WiFiConnected == true )
  {
    const char* host = GetWifiWebHost();

    Serial.printf ( "--- %d -------------------------------- \n\n", value );
    ScanWifi();
    Serial.printf ( "\n" );

    Serial.print ( "Connecting to " );
    Serial.println ( host );

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if ( !client.connect ( host, httpPort ) )
    {
      Serial.println ( "connection failed" );
      return;
    }

    // We now create a URI for the request
    String url = "/servertime.php";
    Serial.print ( "Requesting URL: " );
    Serial.println ( url );

    // This will send the request to the server
    client.print ( String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");
    delay ( 500 );

    // Read all the lines of the reply from server and print them to Serial
    while ( client.available() )
    {
      String line = client.readStringUntil ( '\r' );
      Serial.print ( line );
    }

    Serial.printf ( "\n" );
    Serial.printf ( "Closing connection \n" );
    Serial.printf ( "---------------------------------------- \n\n" );
  }

  else if ( WiFiAPMode == true )
  {
    Serial.printf ( "Stations connected = %d \n", WiFi.softAPgetStationNum() );
    delay ( 3000 );
  }
}

// -----------------------------------------------------------------< /loop >---



// -----------------------------------------------------------------------------
// --------------------------------------------------------------< ScanWifi >---
// -----------------------------------------------------------------------------
//
// PURPOSE:     Scan Wifi for available networks and send report on those found
//              to the serial port.
//
// PARAMETERS:  None
//
// RETURNS:     void
//
// NOTES:
//
// HISTORY:
// --------- ------ ------------------------------------------------------------
// 13Oct2018 DSV    Initial development.
//
// -----------------------------------------------------------------------------

void ScanWifi ()
{
  int NetCount = WiFi.scanNetworks();
  if ( NetCount == 0 )
  {
    Serial.println ( "No wifi networks found" );
  }

  else
  {
    Serial.println ( "Wifi networks found: " + (String) NetCount );

    for ( int i = 0; i < NetCount; i++ )
    {
      // Report the SSID and type of encryption used.
      uint8_t EncryptType = WiFi.encryptionType( i );
      Serial.println ( (String) i
                       + ") SSID = "
                       + (String) WiFi.SSID(i)
                       + "  Encryption Type = "
                       + (String) EncryptType
                       + ( ( EncryptType == 2 ) ? " (WPA)"  :
                           ( EncryptType == 4 ) ? " (WPA2)" :
                           ( EncryptType == 5 ) ? " (WEP)"  :
                           ( EncryptType == 7 ) ? " (NONE)" :
                           ( EncryptType == 8 ) ? " (AUTO)" :
                           " (UNKNOWN)" )
                     );
    }
  }
}

// -------------------------------------------------------------< /ScanWifi >---

