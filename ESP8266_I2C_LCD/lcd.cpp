#include <Wire.h>



/*
  LCD.cpp - Library for using the MIDAS MCCOG21605B6W-FPTLWI 16x2 Black on white 
  LCD, with I2C interface with the MSP430G2553 that comes with the MSP430 Launchpad.
  The I2C interface uses
  MSP4302553|         |LCD
                      |Pin 1 Boost Capacitor 1  
                      |Pin 2 Capacitor 2 N  
                      |Pin 3 Capacitor 2 P
 Vcc (3.5V) |---------|Pin 4 Vdd (Boost Capacitor 1)
        Gnd |---------|Pin 5 Gnd      
        P1.7|---------|Pin 6 (SDA)
        P1.6|---------|Pin 7 (SCL)
        P2.5|---------|Pin 8 (RST)
        P2.4|---------| A (Backlight Anode)
        Gnd |---------| K (Backlight Cathode)

       
  4.7kohm pullups are used on pins 6, 7, 8. Two 1uF capacitors are required
  between pins 1 and 4 and between 2 and 3 when using a 3V supply.
   
  Created by Benn Thomsen, 15 February, 2014.
  Released into the public domain.
*/

#include <Wire.h>
#include "lcd.h"

#define BACKLIGHT 12
#define BLINK 0x01
#define CURSOR 0x02
#define DISPLAYMEM 0x80

LCD::LCD(char address)
{
  _address = address;
  _display=0x00;
  _line=0x00;
}

void LCD::init()
{
      const unsigned char TxData[] =              // Table of data to transmit
  { 0x00,
    0x38,
    0x00,
    0x39,
    0x14,
    0x70,
    0x54,
    0x6F,
    0x0F, //Display On, Cursor On, Cursor Blink On
    0x01
  };
  Wire.begin(); // join i2c bus (address optional for master)
  Wire.beginTransmission(_address);  // transmit to device address 0x3E
  Wire.write(TxData,10);           // send initialisation commands  
  Wire.endTransmission();     // stop transmitting
  _display=0x0F;
  _line=0x00;
  
      /* Define output pin functionality */
  P2DIR |= BIT4;               // Set P1.6 to output
  P2SEL &= ~BIT4;               // Select output P2.4 to be TA1.2
  P2SEL2 &= ~BIT4;             // Select output P2.4 to be TA1.2
  P2OUT |= BIT4;
 
}

void LCD::writeString(char data)
// Writes a string at the current cursor position
{
  Wire.beginTransmission(_address);  // transmit to device #44 (0x2c)
  if (_line == 0x10)
  {
    _line=0x40;
    Wire.write(byte(0x80));           // Send instruction control byte
    Wire.write(DISPLAYMEM | _line);
  }
  Wire.write(byte(0x40));            // Send character control byte
  delay(1);
  Wire.write(data);
  Wire.endTransmission();     // stop transmitting
  _line++;
}

void LCD::writeString(char *data)
// Writes a string at the current cursor position
{
  Wire.beginTransmission(_address);  // transmit to device #44 (0x2c)
  //Wire.write(byte(0x80));            // Send command control
  //Wire.write(_line);                 // Memory Location instruction
  Wire.write(byte(0x40));            // Send character control byte
  delay(1);
  Wire.write(data);
  Wire.endTransmission();     // stop transmitting 
}

void LCD::writeString(char column, char *data)
// Writes a string at the column position specified
{
  Wire.beginTransmission(_address);  // transmit to device #44 (0x2c)
  Wire.write(byte(0x80));            // Send command control
  _line+=column;
  Wire.write(DISPLAYMEM | _line);    // Memory Location instruction
  Wire.write(byte(0x40));            // Send character control byte
  delay(1);
  Wire.write(data);
  Wire.endTransmission();     // stop transmitting 
}

void LCD::clear()
{
  Wire.beginTransmission(_address); // transmit to device #44 (0x2c)
  Wire.write(byte(0x80));            // Send instruction control byte
  Wire.write(0x01);
  Wire.endTransmission();     // stop transmitting
}

void LCD::home()
{
  Wire.beginTransmission(_address); // transmit to device #44 (0x2c)
  Wire.write(0x80);            // Send instruction control byte
  Wire.write(0x02);
  Wire.endTransmission();     // stop transmitting
  _line=0x00;
}

void LCD::blinkOn()
{
  Wire.beginTransmission(_address); // transmit to device #44 (0x2c)
  Wire.write(byte(0x80));           // Send instruction control byte
  _display |=BLINK;
  Wire.write(_display);
  Wire.endTransmission();           // stop transmitting
}

void LCD::blinkOff()
{
  Wire.beginTransmission(_address); // transmit to device #44 (0x2c)
  Wire.write(byte(0x80));           // Send instruction control byte
  _display &= ~BLINK;
  Wire.write(_display);
  Wire.endTransmission();           // stop transmitting
}

void LCD::cursorOn()
{
  Wire.beginTransmission(_address); // transmit to device #44 (0x2c)
  Wire.write(byte(0x80));           // Send instruction control byte
  _display |=CURSOR;
  Wire.write(_display);
  Wire.endTransmission();           // stop transmitting
}

void LCD::cursorOff()
{
  Wire.beginTransmission(_address); // transmit to device #44 (0x2c)
  Wire.write(byte(0x80));           // Send instruction control byte
  _display &= ~CURSOR;
  Wire.write(_display);
  Wire.endTransmission();           // stop transmitting
}

void LCD::lineFeed()
{
  Wire.beginTransmission(_address); // transmit to device #44 (0x2c)
  Wire.write(byte(0x80));           // Send instruction control byte
  _line=0x40;
  Wire.write(DISPLAYMEM | _line);
  Wire.endTransmission();           // stop transmitting
}
void LCD::back()
{
  Wire.beginTransmission(_address); // transmit to device #44 (0x2c)
  Wire.write(byte(0x80));           // Send instruction control byte
  if (_line == 0x00);
  else if (_line == 0x40) _line=0x0F;
  else _line--;
  Wire.write(DISPLAYMEM | _line);
  Wire.endTransmission();           // stop transmitting
}

void LCD::forward()
{
  Wire.beginTransmission(_address); // transmit to device #44 (0x2c)
  Wire.write(byte(0x80));           // Send instruction control byte
  if (_line == 0x0F) _line=0x40;
  else if (_line == 0x4F);
  else _line++;
  Wire.write(DISPLAYMEM | _line);
  Wire.endTransmission();           // stop transmitting
}

void LCD::up()
{
  if (_line & 0x40)
  {
    _line &= 0x0F;
    Wire.beginTransmission(_address); // transmit to device #44 (0x2c)
    Wire.write(byte(0x80));           // Send instruction control byte
    Wire.write(DISPLAYMEM | _line);
    Wire.endTransmission();           // stop transmitting
  }
}

void LCD::down()
{
  if (!(_line & 0x40))
  {
    _line |= 0x40;
    Wire.beginTransmission(_address); // transmit to device #44 (0x2c)
    Wire.write(byte(0x80));           // Send instruction control byte
    Wire.write(DISPLAYMEM | _line);
    Wire.endTransmission();           // stop transmitting
  }
}

void LCD::deleteChar()
{
  Wire.beginTransmission(_address);  // transmit to device #44 (0x2c)
  Wire.write(byte(0x40));            // Send character control byte
  delay(1);
  Wire.write(0x20);
  Wire.endTransmission();     // stop transmitting
  _line++;
}

void LCD::backSpace()
{
  back();
  deleteChar();
  back();
}

void LCD::backLight(char level)
{
  P2SEL |= BIT4;               // Select output P2.4 to be TA1.2
  P2SEL2 &= ~BIT4;             // Select output P2.4 to be TA1.2

  /* Configure timer A as a pulse width modulator */
  TA1CCR0 = 1000-1;             // Set maximum count value to determine PWM Frequency = SMCLK/TACCR0 (1MHz/1000 = 1kHz)
  TA1CCR2 = 999;                // Initialise counter compare value 1 to control Duty Cycle = TACCR1/TACCR0 (500/1000 = 50%)
  TA1CCTL2 = OUTMOD_7;          // Set output to on when counter resets and off when counter equals TACCR1. Normal PWM.
  TA1CTL = TASSEL_2 + MC_1;     // Use the SMCLK to clock the counter and set to count up mode
  TA1CCR2 = level;                // Initialise counter compare value 1 to control Duty Cycle = TACCR1/TACCR0 (500/1000 = 50%)
}
  
