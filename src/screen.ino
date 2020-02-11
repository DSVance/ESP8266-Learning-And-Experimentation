/*********************************************************************
This is an example for a Monochrome OLED based on SSD1306 drivers.

This example is for a 128x64 size display using I2C for communication
between a NodeMCU controller and the SSD1306 display hardware.

Hardware connections:

   NodeMCU GPIO5  (D1)  -->  OLED SCL
   NodeMCU GPIO4  (D2)  -->  OLED SDA
   NodeMCU +3.3   (3V3) -->  VCC
   NodeMCU Ground (GND) -->  GND

*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {                
  Serial.begin(9600);

  // wait for serial monitor to start.
  while ( !Serial ); 

  Serial.println ( "\n\n" );
  Serial.println ( "Drive the SSD1306 OLED display ..." );

  pinMode ( LED_BUILTIN, OUTPUT );

  // Address of the screen in 0x3C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

int counter = 0;
void loop() 
{
  counter++;

  digitalWrite ( LED_BUILTIN, LOW );
  delay ( 250 );
  digitalWrite ( LED_BUILTIN, HIGH );
  Serial.print   ( "Drive loop " );
  Serial.println ( counter );

  // NOTE: You _must_ call "display" after any drawing commands
  // for them to be visible on the display hardware!
  
  // Show whatever the image buffer was initialized with on the display hardware.
  display.clearDisplay();
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  display.setTextColor(WHITE);

  // Text Size 1
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Text 1");
  display.display();
  delay(2000);

  display.setCursor(0,16);
  display.println("Text 1");
  display.display();
  delay(2000);
  display.clearDisplay();

  // Text Size 2
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println("Text 2");
  display.display();
  delay(2000);

  display.setCursor(0,16);
  display.println("Text 2");
  display.display();
  delay(2000);
  display.clearDisplay();

  // Text Size 3
  display.setTextSize(3);
  display.setCursor(0,16);
  display.println("Text 3");
  display.display();
  delay(2000);
  display.clearDisplay();

  display.setCursor(0,24);
  display.println("Text 3");
  display.display();
  delay(2000);
  display.clearDisplay();

  display.setCursor(0,32);
  display.println("Text 3");
  display.display();
  delay(2000);
  display.clearDisplay();

  display.setCursor(0,40);
  display.println("Text 3");
  display.display();
  delay(2000);
  display.clearDisplay();

  // Text Size 4
  display.setTextSize(4);
  display.setCursor(0,16);
  display.println("Txt 4");
  display.display();
  delay(2000);
  display.clearDisplay();  
}

