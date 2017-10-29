/*******************************************************************************************************************
** Class definition header for the INA226 class. This library allows access to the INA226 High-Side or Low-Side   **
** Measurement, Bi-Directional Current and Power Monitor with I2C Compatible Interface. The datasheet can be      **
** download from Texas Instruments at http://www.ti.com/lit/ds/symlink/ina226.pdf.                                **
**                                                                                                                **
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated   **
** documentation files (the "Software"), to deal in the Software without restriction, including without limitation**
** the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,   **
** and to permit persons to whom the Software is furnished to do so, subject to the following conditions:         **
** The above copyright notice and this permission notice shall be included in all copies or substantial portions  **
** of the Software.                                                                                               **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
** Thanks to SV-Zanshin for his initial work on this project                                                      **
** If you would like an EEPROM based version see his Github at https://github.com/SV-Zanshin/INA226               **
** This version holds everything in a structure array as EEPROM memory is usually limited and slower to update    **
*******************************************************************************************************************/
#include <DBOE INA226.h>

#define INA226_ALERT_PIN 8                                                     // Pin 8. Micro only allows SS,SCK
#define GREEN_LED_PIN 13                                                       // Green LED
#define INA1 0x40                                                              // INA226 I2C Address
#define debug true                                                             // if True serial prints will occur

//Declare global variables and instantiate classes
INA226_Class INA226;                                                           // INA class instantiation
volatile uint64_t sumBusMillVolts =      0;                                    // Sum of bus voltage readings
volatile int64_t  sumBusMicroAmps =      0;                                    // Sum of bus amperage readings
volatile uint8_t  readings        =      0;                                    // Number of measurements taken

//Declare interrupt service routine for the pin-change interrupt on pin 8 which is set in the setup() method
ISR (PCINT0_vect) {
  digitalWrite(GREEN_LED_PIN,!digitalRead(GREEN_LED_PIN));                     // Toggle LED to show we are working
  *digitalPinToPCMSK(INA226_ALERT_PIN)&=~bit(digitalPinToPCMSKbit(INA226_ALERT_PIN)); // Disable PCMSK pin
  PCICR &= ~bit(digitalPinToPCICRbit(INA226_ALERT_PIN));                       // disable interrupt for the group
  sei();                                                                       // Enable interrupts for I2C calls
  sumBusMillVolts += INA226.getBusMilliVolts(INA1);                            // Read the current value
  sumBusMicroAmps += INA226.getBusMicroAmps(INA1);                             // Read the current value
  readings++;                                                                  // Increment the number of readings
  INA226.waitForConversion(INA1);                                              // Resets INA226 interrupt flag
  cli();                                                                       // Disable interrupts
  *digitalPinToPCMSK(INA226_ALERT_PIN)|=bit(digitalPinToPCMSKbit(INA226_ALERT_PIN)); // Enable PCMSK pin
  PCIFR  |= bit (digitalPinToPCICRbit(INA226_ALERT_PIN));                      // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(INA226_ALERT_PIN));                      // enable interrupt for the group
}

void setup() {
  pinMode(GREEN_LED_PIN, OUTPUT);                                              // Define the green LED as an output
  digitalWrite(GREEN_LED_PIN,true);                                            // Turn on the LED
  pinMode(INA226_ALERT_PIN,INPUT_PULLUP);                                      // Declare pin with pullup resistors
  *digitalPinToPCMSK(INA226_ALERT_PIN)|=bit(digitalPinToPCMSKbit(INA226_ALERT_PIN)); // Enable PCMSK pin
  PCIFR  |= bit (digitalPinToPCICRbit(INA226_ALERT_PIN));                      // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(INA226_ALERT_PIN));                      // enable interrupt for the group
  Serial.begin(115200);                                                        // Start serial communications

  #ifdef  __AVR_ATmega32U4__                                                   // If this is a 32U4 processor,
    delay(3000);                                                               // wait 3 seconds for serial port
  #endif                                                                       // interface to initialize

  if(debug) Serial.println("Background INA226 Read V1.0.0");                   // Display program information

  // The begin initializes with expected 819 mA maximum current and for a 0.1 Ohm (100000 microOhm) resistor,
  // 1 device connected, I2C address of 0x40 (INA1). This assumes a ebay special INA226 board
  INA226.begin(819,100000,0,INA1);
  INA226.setAveraging(64);                                                     // Average each reading n-times
  INA226.setBusConversion(4);                                                  // Maximum conversion time 1.1ms
  INA226.setShuntConversion(4);                                                // Maximum conversion time 1.1ms
  INA226.setMode(INA_MODE_CONTINUOUS_BOTH);                                    // Bus/shunt measured continuously
  INA226.setAlertPinOnConversion(true);                                        // Make alert pin go low on finish
}

void loop() {
  static long lastMillis = millis();                                           // Store the last time we printed
  /*****************************************************************************************************************
  ** Check to see if we have collected 10 or more readings each main loop iteration, and display the time and     **
  ** average information before resetting the values. Interrupts are turned off when resetting the values to      **
  ** ensure atomic operations                                                                                     **
  *****************************************************************************************************************/
  if (readings>=10) {                                                          // If it is time to display results
    if(debug) Serial.print("Averaging readings over: ");
    if(debug) Serial.print((float)(millis()-lastMillis)/1000,2);
    if(debug) Serial.print(" seconds. Bus Volts: ");
    if(debug) Serial.print((float)sumBusMillVolts/readings/1000.0,4);
    if(debug) Serial.print("V Bus amperage: ");
    if(debug) Serial.print((float)sumBusMicroAmps/readings/1000.0,4);
    if(debug) Serial.println("mA"));
    if(debug) Serial.println("-------------------------------------------------------------------");
    lastMillis = millis();
    cli();                                                                     // Disable interrupts
    readings        = 0;                                                       // Reset values
    sumBusMillVolts = 0;                                                       // Reset values
    sumBusMicroAmps = 0;                                                       // Reset values
    sei();                                                                     // Enable interrupts
  }
}
