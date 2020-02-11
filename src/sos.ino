void setup()
{
   // Configure the onboard LED pin for output.
   pinMode ( LED_BUILTIN, OUTPUT );
}

void repeat_blink( int Count, int Pause )
{
   // Blink the onboard LED the specified number
   // of times, at the specified rate.

   if ( Count > 0 and Pause > 0 )
   {
      for ( int i = 0; i < Count; i++ )
      {
         digitalWrite ( LED_BUILTIN, LOW );
         delay ( Pause );
         digitalWrite ( LED_BUILTIN, HIGH );
         delay ( Pause );
      }
   }
}

void loop()
{
   // Main code runs repeatedly.
   int LongPause  = 300;
   int ShortPause = LongPause / 2;

   // S.O.S in morse code is:  ...---...
   repeat_blink ( 3, ShortPause );
   delay ( LongPause );
   repeat_blink ( 3, LongPause );
   delay ( ShortPause );
   repeat_blink ( 3, ShortPause );

   delay ( 3000 );
}
