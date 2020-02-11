// I2c_post_address_scanner
//
// Scans ports D0 to D7 on an ESP8266 and searches for I2C device.  
// D8 throws exceptions thus it has been left out.
//
// Example output:
// Scanning (SDA : SCL) - GPIO5 : GPIO0 - No I2C devices found
// Scanning (SDA : SCL) - GPIO5 : GPIO2 - No I2C devices found
// Scanning (SDA : SCL) - GPIO5 : GPIO4 - I2C device found at address 0x3C
// Scanning (SDA : SCL) - GPIO5 : GPIO12 - No I2C devices found
// Scanning (SDA : SCL) - GPIO5 : GPIO13 - No I2C devices found

//
// https://www.instructables.com/id/ESP8266-I2C-PORT-and-Address-Scanner/


#include <Wire.h>


uint8_t  portArray[] = { 0, 2, 4, 5, 12, 13, 14, 16 };
String   portMap[] = { "GPIO0", "GPIO2", "GPIO4", "GPIO5", "GPIO12", "GPIO13", "GPIO14", "GPIO16", };
uint32_t Scans = 0;


void setup() 
{
   Serial.begin ( 115200 );

   // Configure the onboard LED pin for output.
   pinMode ( LED_BUILTIN, OUTPUT );

   // wait for serial monitor to start.
   while ( !Serial ); 
   
   Serial.println ( "\n\nI2C Scanner to scan for devices on each port pair D0 to D7" );
}


void scanPorts()
{ 
   for ( uint8_t i = 0; i < sizeof ( portArray ); i++ )
   {
      for ( uint8_t j = 0; j < sizeof ( portArray ); j++ )
      {
         if ( i != j )
         {
            Serial.print ( "Scanning (SDA : SCL) - " + portMap[i] + " : " + portMap[j] + " - " );

            Wire.begin ( portArray[i], portArray[j] );

            check_if_exist_I2C();
         }
      }
   }
}


void check_if_exist_I2C()
{
   byte  error;
   byte  address;
   int   nDevices = 0;

   for ( address = 1; address < 127; address++ )
   {
      // Use the return value of Write.endTransmisstion to
      // see if a device acknowledges at the address.
      Wire.beginTransmission ( address );

      error = Wire.endTransmission();

      if ( error == 0 )
      {
         Serial.print ( "I2C device found at address 0x" );
         if ( address < 16 )
         {
            Serial.print("0");
         }

         Serial.println ( address, HEX );
         nDevices++;
      }

      else if ( error == 4 )
      {
         Serial.print ( "Unknow error at address 0x" );

         if ( address < 16 )
         {
            Serial.print("0");
         }

         Serial.println ( address, HEX );
      }
   }

   if ( nDevices == 0 )
   {
      Serial.println ( "No I2C devices found" );
   }

//   else
//   {
//      Serial.println ( "**********************************\n" );
//   }
}


void loop() 
{
   Scans++;
   
   digitalWrite ( LED_BUILTIN, LOW );
   delay ( 250 );
   digitalWrite ( LED_BUILTIN, HIGH );
   
   Serial.print   ( "\n*** Scan " );
   Serial.print   ( Scans );
   Serial.println ( " **********************************" );
   scanPorts();
   delay ( 20000 );
}

