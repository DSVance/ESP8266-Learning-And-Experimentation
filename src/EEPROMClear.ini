
#include <EEPROM.h>              // Read a write to permanent FLASH storage


void ClearROM ( uint32_t Size );


void setup() 
{
   int      Size = 512;
   uint16_t Tries;

   // open the serial port
   Serial.begin ( 115200 );
   Tries = 0;
   while ( !Serial && Tries < 30 )
   {
      Tries++;
      delay ( 500 );
   }

   // Configure the onboard LED pin for output.
   pinMode ( LED_BUILTIN, OUTPUT );

   for ( int i = 0; i < 3; i++ )
   {
      digitalWrite ( LED_BUILTIN, LOW );
      delay ( 150 );
      digitalWrite ( LED_BUILTIN, HIGH );
      delay ( 150 );
   }

   Serial.printf ( "\n\nStarting EEPROM clear of size %d \n", Size );

   delay ( 500 );

   // Clear the EEPROM
   for ( int i = 0; i < Size; i++ ) 
   {
      EEPROM.write ( i, 0 );
   }
   EEPROM.commit();

   delay ( 500 );

   Serial.println ( "Finished EEPROM clear" );

   for ( int i = 0; i < 3; i++ )
   {
      digitalWrite ( LED_BUILTIN, LOW );
      delay ( 300 );
      digitalWrite ( LED_BUILTIN, HIGH );
      delay ( 300 );
   }
}


void loop() 
{
   // Do nothing;
}
