/*  CMSC Chaff/Flare Display and MWS Display
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
 *    Two HDSP-2531 being wired to two 74HC595N
 *    Do this for each HDSP chip
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

#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"

// HDSP-2531 Chip #1 
const int RST   = A0; // wire to HDSP-2531 pin 1  (share amoungst the two chips (wire splice))
const int WR    = A1; // wire to HDSP-2531 pin 13
const int CE    = 10; // wire to HDSP-2531 pin 14
const int AD0   = A2; // wire to HDSP-2531 pin 3
const int AD1   = A3; // wire to HDSP-2531 pin 4
const int AD2   = A4; // wire to HDSP-2531 pin 5
// PIN A5 is free
const int DS    = 2;  // wire to 74HC595N  pin 14 shift register data pin
const int ST_CP = 3;  // wire to 74HC595N  pin 12 shift register storage clock(latch)
const int SH_CP = 4;  // wire to 74HC595N  pin 11 shift register shift clock(data polling)

// HDSP-2531 Chip #2
const int WR_2    = 5; // wire to HDSP-2531 pin 13
const int CE_2    = 6; // wire to HDSP-2531 pin 14
const int AD0_2   = 7; // wire to HDSP-2531 pin 3
const int AD1_2   = 8; // wire to HDSP-2531 pin 4
const int AD2_2   = 9; // wire to HDSP-2531 pin 5
const int DS_2    = 11;  // wire to 74HC595N  pin 14 shift register data pin
const int ST_CP_2 = 12;  // wire to 74HC595N  pin 12 shift register storage clock(latch)
const int SH_CP_2 = 13;  // wire to 74HC595N  pin 11 shift register shift clock(data polling)

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

void setup() {
  DcsBios::setup();
     pinMode(RST, OUTPUT);
     pinMode(WR, OUTPUT);
     pinMode(WR_2, OUTPUT);
     pinMode(CE, OUTPUT);
     pinMode(CE_2, OUTPUT);
     pinMode(AD0, OUTPUT);   // data register selection // these 4 pins allow you to select a data register.
     pinMode(AD0_2, OUTPUT);
     pinMode(AD1, OUTPUT);   // data register selection
     pinMode(AD1_2, OUTPUT);
     pinMode(AD2, OUTPUT);   // data register selection
     pinMode(AD2_2, OUTPUT);
     pinMode(DS, OUTPUT);    // shift register serial in 
     pinMode(DS_2, OUTPUT);
     pinMode(ST_CP, OUTPUT); // shift register storage pin(latch)
     pinMode(ST_CP_2, OUTPUT);
     pinMode(SH_CP, OUTPUT); // shift register clock pin(clock)
     pinMode(SH_CP_2, OUTPUT);
     digitalWrite(CE, HIGH); // chip enable, go low before write. go high after write
     digitalWrite(CE_2, HIGH);
     digitalWrite(WR, HIGH); // write enable, go low to start write, go high when done.
     digitalWrite(WR_2, HIGH);
     resetDisplay();
}

void resetDisplay()
{
     digitalWrite(RST, LOW);
     delayMicroseconds(1);
     digitalWrite(RST,HIGH);
     delayMicroseconds(150);
}

void writeChaffDisplay(char *input)
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

void writeMWSDisplay(char *input)
{
     for (int i=0; i<8; i++)
     {
          digitalWrite(AD0_2, (1&i)!=0?HIGH:LOW); // character address because there are 8 alphanumeric digits
          digitalWrite(AD1_2, (2&i)!=0?HIGH:LOW); // 000 is digit on far left. 111 is digit on far right
          digitalWrite(AD2_2, (4&i)!=0?HIGH:LOW);
          delay(1);
          digitalWrite(ST_CP_2, LOW);         
          delay(1);
          shiftOut(DS_2, SH_CP_2, MSBFIRST, input[i]);
          delay(1);
          digitalWrite(ST_CP_2, HIGH);
          delay(1);
          digitalWrite(CE_2, LOW);
          delay(1);
          digitalWrite(WR_2, LOW);
          delay(1); 
          digitalWrite(WR_2, HIGH);
          delay(1);
          digitalWrite(CE_2, HIGH);
          delay(1);
     }
}

void loop() {
  DcsBios::loop();
}

// Chaff / Flare Amount Display
void onCmscTxtChaffFlareChange(char* newValue) {
     writeChaffDisplay(newValue);
}
DcsBios::StringBuffer<8> cmscTxtChaffFlareBuffer(0x108e, onCmscTxtChaffFlareChange);

// MWS Status Display
void onCmscTxtMwsChange(char* newValue) {
    writeMWSDisplay(newValue);
}
DcsBios::StringBuffer<8> cmscTxtMwsBuffer(0x12b0, onCmscTxtMwsChange);
