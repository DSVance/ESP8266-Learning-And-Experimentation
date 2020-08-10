// -----------------------------------------------------------------------------
// -----------------------------------------------< NetworkTimeProtocol.ino >---
// -----------------------------------------------------------------------------
//
// PROJECT: Arduino development for the ESP8266 custom temperature sensor board.
//
// PURPOSE: Retrieve the current time using Network Time Protocol (NTP).
//
// AUTHOR:  Scott Vance
//
// NOTES:   NTP communicates over the network using UDP.  Unlike TCP, UDP does
//          not use a persistent connection.  A client sends a request message 
//          to a server, but does not suspend waiting for a response.  If there 
//          is no response in some arbitrary (but fixed) interval, the client 
//          simply sends another request.  For its part, the NTP server sends a 
//          response to the client whenever it is ready.
//
//          With UDP there is no guarantee that either the request or the 
//          response will reach their destination, and no way to know whether 
//          or not they arrived.
//
// HISTORY:
// --------- ----------- - -----------------------------------------------------
// 20Feb2019 Scott Vance - Initial development.
//
// -----------------------------------------------------------------------------


#include <ESP8266WiFi.h>         // Basic wifi connection services.
#include <WiFiUdp.h>             // NTP messages are transmitted over UDP.
#include <WifiConnect.h>         // Wifi network ID and credentials.
#include <assert.h>              // Assert statements


bool        WiFiConnected = false;

// The port that an NTP server listens for request on is always 123.
const int   NTP_SERVER_PORT = 123;

// The first 48 bytes of an NTP message contains the time stamp.
const int   NTP_PACKET_SIZE = 48;

byte        UDPBuffer[ NTP_PACKET_SIZE ];

const char* NTPServerName = "time.nist.gov";

IPAddress   NTPServerIP;



// A UDP instance to let us send and receive packets over UDP
WiFiUDP UDPConnection;



// Function Prototypes
void SendNTPRequest ( IPAddress& NTPAddress );
void ShowHex ( int BufferSize, int LineLength = 8 );



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
// 20Feb2019 Scott Vance - Initial development.
//
// -----------------------------------------------------------------------------

void setup()
{
   int         Status;
   const char* ssid      = GetWifiSSID();
   const char* password  = GetWifiPW();
   uint16_t    Tries;

   // A local port to listen for UDP packets on.
   unsigned int   NTPPort = 5760;


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

      Status = UDPConnection.begin ( NTPPort );
      if ( Status == 0 )
      {
         Serial.printf ( "UDP services failed to start on local port %d.  Rebooting. \n", NTPPort );
         Serial.flush();
         ESP.reset();
      }         
      
      Serial.printf ( "Started UDP services on local port %d \n",
                      UDPConnection.localPort()
                    );

      // Lookup the IP address of the NTP server by its host name.
      Status = WiFi.hostByName ( NTPServerName, NTPServerIP );
      if ( Status == 0 )
      {
         Serial.printf ( "IP lookup for \"%s\" failed.  Rebooting. \n", NTPServerName );
         Serial.flush();
         ESP.reset();
      }

      Serial.printf  ( "NTP server name: %s  IP: ", NTPServerName );
      Serial.println ( NTPServerIP );
      Serial.println ();
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
// 20Feb2019 Scott Vance - Initial development.
//
// -----------------------------------------------------------------------------

void loop()
{
   digitalWrite ( LED_BUILTIN, LOW );
   delay ( 1000 );
   digitalWrite ( LED_BUILTIN, HIGH );

   if ( WiFiConnected == true )
   {
      int RecievedPacketSize;
      
      // Send a time stamp request to the returned NTP server.
      SendNTPRequest ( NTPServerIP );

      // Give the server a chance to respond.
      delay ( 500 );


      RecievedPacketSize = UDPConnection.parsePacket();

      if ( !RecievedPacketSize )
      {
         Serial.println ( "No packet returned yet ..." );
      }

      else
      {
         char  Hours;
         char  Minutes;
         char  Seconds;

         // UTC Time Zone Differance
         char ZoneHourOffset = 6;
         char ZoneMinuteOffset = 0;


         // Read all of the data from it
         UDPConnection.read ( UDPBuffer, NTP_PACKET_SIZE );

         Serial.printf ( "Recieved NTP response of %d bytes: \n", RecievedPacketSize );
         ShowHex ( RecievedPacketSize );
         
         // The timestamp starts at byte 40 of the received packet and is four bytes,
         // or two words, long. First, esxtract the two words:
         unsigned int highWord = word ( UDPBuffer[40], UDPBuffer[41] );
         unsigned int lowWord  = word ( UDPBuffer[42], UDPBuffer[43] );

         // Combine the four bytes (two words) into a long integer
         // this is NTP time (seconds since Jan 1 1900):
         unsigned int NTPTime = highWord << 16 | lowWord;


         Serial.printf ( "(UTC)  Seconds since Jan 1 1900 = %u \n", NTPTime );


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
         const unsigned int NTP2Unix = 2208988800UL;

         // Subtract seventy years to get Unix time.
         unsigned int UnixTime = NTPTime - NTP2Unix;

         // Print Unix time
         Serial.printf ( "(Unix) Seconds since Jan 1 1970 = %u \n", UnixTime );


         // print the hour, minute and second:
         Seconds = ( UnixTime % 60 );

         Minutes = ( ( UnixTime % 3600 ) / 60 );
         Minutes = Minutes + ZoneMinuteOffset; //Add UTC Time Zone

         Hours = ( UnixTime  % 86400L ) / 3600;

         
         if ( Minutes > 59 )
         {
            Hours = Hours + ZoneHourOffset + 1; //Add UTC Time Zone
            Minutes = Minutes - 60;
         }

         else
         {
            Hours = Hours + ZoneHourOffset;
         }

         Serial.printf ( "The UTC time + %02d:%02d zone offset is %02d:%02d:%02d \n", 
                         ZoneHourOffset, 
                         ZoneMinuteOffset,
                         Hours,
                         Minutes,
                         Seconds
                       );
      }

      // Wait ten seconds before getting the next NTP time stamp.
      delay ( 10000 );
   }
}

// -----------------------------------------------------------------< /loop >---



// -----------------------------------------------------------------------------
// --------------------------------------------------------< SendNTPRequest >---
// -----------------------------------------------------------------------------
//
// PURPOSE:     Send a UDP request message to an NTP server requesting the time.
//
// PARAMETERS:  NTPAddress - The IP address of the server to send to.
//
// RETURNS:     void
//
// NOTES:
//
// HISTORY:
// --------- ----------- -------------------------------------------------------
// 20Feb2019 Scott Vance - Initial development.
//
// -----------------------------------------------------------------------------

void SendNTPRequest ( IPAddress& NTPAddress )
{

   // set all bytes in the buffer to 0
   memset ( UDPBuffer, 0, NTP_PACKET_SIZE );

   // Initialize values needed to form NTP request

   UDPBuffer[  0 ] = 0b11100011;    // LI, Version, Mode
   UDPBuffer[  1 ] = 0;             // Type of clock
   UDPBuffer[  2 ] = 6;             // Polling Interval
   UDPBuffer[  3 ] = 0xEC;          // Peer Clock Precision

   // 8 bytes of zero for Root Delay & Root Dispersion
   
   UDPBuffer[ 12 ]  = 49;
   UDPBuffer[ 13 ]  = 0x4E;
   UDPBuffer[ 14 ]  = 49;
   UDPBuffer[ 15 ]  = 52;

   Serial.printf ( "Sending NTP request of %d bytes: \n", NTP_PACKET_SIZE );
   ShowHex ( NTP_PACKET_SIZE );
   
   UDPConnection.beginPacket ( NTPAddress, NTP_SERVER_PORT );
   UDPConnection.write ( UDPBuffer, NTP_PACKET_SIZE );
   UDPConnection.endPacket();
}

// -------------------------------------------------------< /SendNTPRequest >---



// -----------------------------------------------------------------------------
// ---------------------------------------------------------------< ShowHex >---
// -----------------------------------------------------------------------------
//
// PURPOSE:    Print the bytes from a buffer to the serial port in HEX format.
//
// PARAMETERS: BufferSize - The size (number of bytes) of the buffer to print.
//
//             LineLength - (optional) The number of bytes to show on each line.
//
// RETURNS:    void
//
// NOTES:      -  The output is broken in lines with a specified number of bytes
//                shown on each line.  The default number of bytes per line is 
//                eight if no other line length is specified.
//
// HISTORY:
// --------- ----------- -------------------------------------------------------
// 20Feb2019 Scott Vance - Initial development.
//
// -----------------------------------------------------------------------------

void ShowHex ( int BufferSize, int LineLength )
{
   for ( int i = 0; i < (BufferSize / LineLength ); i++ )
   {
      Serial.printf ( "   " );
      for ( int j = 0; j < LineLength; j++ )
      {
         Serial.printf ( "%02X ", UDPBuffer[ (i * LineLength) + j ] ); 
      }
      Serial.println();
   }
}

// --------------------------------------------------------------< /ShowHex >---

