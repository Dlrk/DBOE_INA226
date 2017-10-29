/*******************************************************************************************************************
** Program to demonstrate the INA226 library for the Arduino IDE. A simple infinite loop of measurments will      **
** display the bus voltage and current running through the INA226.                                                **
**                                                                                                                **
** This example is for a INA226 set up to measure a 819mA load with a 0.1 Ohm resistor in place, this is the same **
** setup that can be found in the ebay INA226 breakout boards.                                                    **
**                                                                                                                **
** The datasheet for the INA226 can be found at http://www.ti.com/lit/ds/symlink/ina226.pdf and it contains the   **
** information required in order to hook up the device.                                                           **
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
#include <DBOE_INA226.h>

// Declare global variables and instantiate classes
INA226_Class INA226;                                                           // INA class instantiation
#define INA1 0x40                                                              // INA226 I2C Address

void setup() {
  Serial.begin(115200);                                                        // Start serial communications

  #ifdef  __AVR_ATmega32U4__                                                   // If we are a 32U4 processor,
    delay(2000);                                                               // then wait 2 seconds for serial
  #endif                                                                       // interface to initialize

  // The begin initializes with expected 819 mA maximum current and for a 0.1 Ohm (100000 microOhm) resistor,
  // 1 device connected, I2C address of 0x40 (INA1). This assumes a ebay special INA226 board
  INA226.begin(819,100000, 0, INA1);
  INA226.setAveraging(16);                                                     // Average each reading n-times
  INA226.setBusConversion(4);                                                  // Maximum conversion time 1.1ms
  INA226.setShuntConversion(4);                                                // Maximum conversion time 1.1ms
  INA226.setMode(INA_CONTINUOUS_BOTH);                                         // Bus/shunt measured continuously
}

void loop() {
  Serial.print("Bus Volts: ");
  Serial.print((float)INA226.getBusMilliVolts(INA1)/1000.0,4);                 // Convert to millivolts
  Serial.print("V Shunt Volts: ");
  Serial.print((float)INA226.getShuntMicroVolts(INA1)/1000.0,3);               // Convert to millivolts
  Serial.print("mV Bus Amps: ");
  Serial.print((float)INA226.getBusMicroAmps(INA1)/1000.0,3);                  // Convert to milliamp
  Serial.print("mA Bus Watts: ");
  Serial.print((float)INA226.getBusMicroWatts(INA1)/1000.0,3);                 // Convert to milliwatts
  Serial.println("mW");
  delay(1000);
}
