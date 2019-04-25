/*  CMSC Jammer Display
 *   
 *  Based on code done by  
 *  Matt Joyce < matt at nycresistor.com >
 *  Mark Tabry
 *  and
 *  Jos van Dijken
 *  www.josvandijken.nl
 *  28 april 2012
 *  
 *  Modified for use with DCS Bios by Joe Sim
 *  For use with Arduino Nano, Micro, Uno, Mega
 *  DCS World version 2.5.4.xxxxx
 *  DCS BIOS version 0.7.1
 *  Arduino IDE version 1.8.5
 *  GSS Rain in DCS Forums
 *  Joe Sim in Youtube
 *  https://www.youtube.com/watch?v=O8CsBrvS3NM&t=1s
 *  
 *  Trouble-shooting notes, if power wire comes loose from Terminal Board then it not work right.
 *  Use an external 5VDC Power Supply because the HDSP and 74HC595 can pull too much
 *  current for the arduino to supply.
 *  Also place a 0.1uF or 0.01uF Capacitor (103 or 104) on power pin of each chip. 
 *  
 *    WIRING DIAGRAM CONNECTION TABLE
 *    HDSP-2531 being wired to 74HC595N
 *    HDSP-2531 pin 2  "/Flash Ram"  wire to 5VDC
 *    HDSP-2531 pin 6  "A3" wire to 5VDC
 *    HDSP-2531 pin 10 "A4" wire to 5VDC
 *    HDSP-2531 pin 11 "Clock Source" wire to 5VDC
 *    HDSP-2531 pin 12 "Clock In/Out" - No Connect
 *    HDSP-2531 pin 17 "Thermal Test" - No Connect   
 *    HDSP-2531 pin 15 "Vcc" wire to 5VDC
 *    HDSP-2531 pin 16 "GND" wire to Ground
 *    HDSP-2531 pin 18 "GND" wire to Ground
 *    HDSP-2531 pin 19 "Read" - No Connect
 */

// If using Arduino Micro
// #define DCSBIOS_DEFAULT_SERIAL

// For all others arduinos
#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"
 
const int RST   = A0; // HDSP-2531 pin 1
const int WR    = A1; // HDSP-2531 pin 13
const int CE    = 10; // HDSP-2531 pin 14
const int AD0   = A2; // HDSP-2531 pin 3
const int AD1   = A3; // HDSP-2531 pin 4
const int AD2   = A4; // HDSP-2531 pin 5
// pin A5 is free
const int DS    = 2;  // 74HC595N  pin 14 shiftregister data pin
const int ST_CP = 3;  // 74HC595N  pin 12 shiftregister storageclock(latch)
const int SH_CP = 4;  // 74HC595N  pin 11 shiftregister shiftclock(data polling)

/* 
 *  HDSP-2531 to 74HC595N wiring 
 *  HDSP-2531 "D0" pin 20 to 74HC595N pin 15
 *  HDSP-2531 "D1" pin 21 to 74HC595N pin 1
 *  HDSP-2531 "D2" pin 25 to 74HC595N pin 2 
 *  HDSP-2531 "D3" pin 26 to 74HC595N pin 3   
 *  HDSP-2531 "D4" pin 27 to 74HC595N pin 4 
 *  HDSP-2531 "D5" pin 28 to 74HC595N pin 5  
 *  HDSP-2531 "D6" pin 29 to 74HC595N pin 6  
 *  HDSP-2531 "D7" pin 30 to 74HC595N pin 7  
 *  74HC595N "/MasterReset" pin 10 to 5V
 *  74HC595N "/OutputEnable" pin 13 to GND
 *  74HC595N "Data Out" pin 9 to No Connect
 *  74HC595N "Ground" pin 8 to GND
 *  74HC595N "Vcc" pin 16 to 5V
*/

// Select Jammer Program
DcsBios::Switch2Pos cmscJmr("CMSC_JMR", 5);

// Select MWS Programs (No Function)
DcsBios::Switch2Pos cmscMws("CMSC_MWS", 6);

// Toggle between 5 and 16 Priority Threats Displayed
DcsBios::Switch2Pos cmscPri("CMSC_PRI", 7);

// Separate RWR Symbols
DcsBios::Switch2Pos cmscSep("CMSC_SEP", 8);

// Missile Launch Indicator
DcsBios::LED cmscLaunch(0x1012, 0x0100, 9);

// Priority Status Indicator
DcsBios::LED cmscPrio(0x1012, 0x0200, 11);

// Unknown Status Indicator
DcsBios::LED cmscUnkn(0x1012, 0x0400, 12);

// Adjust Display Brightness
// DcsBios::Potentiometer cmscBrt("CMSC_BRT", A6);

// Adjust RWR Volume
// DcsBios::Potentiometer cmscRwrVol("CMSC_RWR_VOL", A7);

void setup() {
  DcsBios::setup();
     pinMode(RST, OUTPUT);
     pinMode(WR, OUTPUT);
     pinMode(CE, OUTPUT);
     pinMode(AD0, OUTPUT);
     pinMode(AD1, OUTPUT);
     pinMode(AD2, OUTPUT);
     pinMode(DS, OUTPUT);    // shift register serial in 
     pinMode(ST_CP, OUTPUT); // shift register storage pin(latch)
     pinMode(SH_CP, OUTPUT); // shift register clock pin(clock)          
     digitalWrite(CE, HIGH);
     digitalWrite(WR, HIGH);
     resetDisplay();
}

void resetDisplay()
{
     digitalWrite(RST, LOW);
     delayMicroseconds(1);
     digitalWrite(RST,HIGH);
     delayMicroseconds(150);
}

void writeJammerDisplay(char *input)
{
     for (int i=0; i<8; i++)
     {
          digitalWrite(AD0, (1&i)!=0?HIGH:LOW); // character address because there are 8 alphanumeric digits
          digitalWrite(AD1, (2&i)!=0?HIGH:LOW); // 000 is digit on far left. 111 is digit on far right
          digitalWrite(AD2, (4&i)!=0?HIGH:LOW);
          delay(1);
          digitalWrite(ST_CP, LOW);          
          delay(1);
          shiftOut(DS, SH_CP, MSBFIRST, input[i]);          
          delay(1);
          digitalWrite(ST_CP, HIGH);          
          delay(1);         
          digitalWrite(CE, LOW);
          delay(1);
          digitalWrite(WR, LOW);
          delay(1);          
          digitalWrite(WR, HIGH);
          delay(1);
          digitalWrite(CE, HIGH);
          delay(1);   
     }
}

void loop() {
  DcsBios::loop();
}

// JMR Status Display
void onCmscTxtJmrChange(char* newValue) {
     writeJammerDisplay(newValue);
}
DcsBios::StringBuffer<8> cmscTxtJmrBuffer(0x1096, onCmscTxtJmrChange);

