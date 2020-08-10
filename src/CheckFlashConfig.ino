/*
  ESP8266 CheckFlashConfig by Markus Sattler

  This sketch tests if the EEPROM settings of the IDE match to the Hardware

  Modified to pass args from setup to loop, and to loop only three times.
*/

void* setup ( void ) 
{
   Serial.begin ( 115200 );
   
   uint16_t Tries = 0;
   while ( !Serial && Tries < 30 )
   {
      Tries++;
      delay ( 500 );
   }
  
   Serial.println ( "\n" );
}

void loop ( void* Args ) 
{
   for ( static int i = 0; i < 3; i++ )
   {
      uint32_t    ChipID        = ESP.getChipId();
      uint32_t    FlashID       = ESP.getFlashChipId();
      uint32_t    FlashSizeReal = ESP.getFlashChipRealSize();
      uint32_t    FlashSizeIDE  = ESP.getFlashChipSize();
      FlashMode_t IDEMode       = ESP.getFlashChipMode();

      Serial.printf ( "---< %d >----------------------------------- \n\n", i + 1 );

      Serial.printf ( "Chip ID = %d (0x%08x) \n\n", ChipID, ChipID );

      Serial.printf ( "Flash real ID:   %d (0x%08x) \n", FlashID, FlashID );
      Serial.printf ( "Flash real size: %u bytes (%1.2f Kb) \n\n", FlashSizeReal, (float) FlashSizeReal / 1024.0 );

      Serial.printf ( "Flash IDE  size: %u bytes (%1.2f Kb) \n", FlashSizeIDE, (float) FlashSizeIDE / 1024.0 );
      Serial.printf ( "Flash IDE speed: %u Hz \n", ESP.getFlashChipSpeed() );
      Serial.printf ( "Flash IDE mode:  %s \n", ( IDEMode == FM_QIO ? "QIO"  : IDEMode == FM_QOUT 
                                                                    ? "QOUT" : IDEMode == FM_DIO 
                                                                    ? "DIO"  : IDEMode == FM_DOUT 
                                                                    ? "DOUT" : "UNKNOWN" ));

      if ( FlashSizeIDE != FlashSizeReal ) 
      { 
         Serial.println ( "\nFlash Chip configuration does not match IDE! \n" );
      } 
   
      else 
      {
         Serial.println ( "\nFlash Chip configuration matches IDE. \n" );
      }

      if ( i == 2 )
      {
         Serial.printf ( "---< Done >--------------------------------- \n\n" );
      }

      delay ( 5000 );
   }
}

