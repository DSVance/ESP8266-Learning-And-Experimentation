
// GPIO13 is on board pin labeled D7
int LED_D7 = 13;


void setup()
{
   // Configure the onboard LED pin for output.
   pinMode ( LED_BUILTIN, OUTPUT );

   // Configure GPIO13/D7 for output.
   pinMode ( LED_D7, OUTPUT );

   // open the serial port at 9600 bps.
   Serial.begin ( 9600 );
}


void loop()
{
   // Turn the onboard LED on and D7 off.
   Serial.print ( "LED - ON \n" );
   digitalWrite ( LED_BUILTIN, LOW );
   digitalWrite ( LED_D7, LOW );
   // Leave it on for 1 second.
   delay ( 1000 );

   // Turn the onboard LED off and D7 on.
   Serial.print ( "LED - OFF \n" );
   digitalWrite ( LED_BUILTIN, HIGH );
   digitalWrite ( LED_D7, HIGH );

   // Leave the current state for 5 seconds.
   Serial.print ( "Waiting ... \n" );
   delay ( 5000 );
}
