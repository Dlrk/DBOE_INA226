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
#ifndef DBOE_INA226_h
#define DBOE_INA226_h
#define LIBRARY_VERSION  1.0.0

#if ARDUINO >= 100
 #include "Arduino.h"
 #define WIRE_WRITE Wire.write
 #define WIRE_READ Wire.read
#else
 #include "WProgram.h"
 #define WIRE_WRITE Wire.send
 #define WIRE_READ Wire.receive
#endif

/*****************************************************************************************************************
** Declare constants used in the class                                                                          **
*****************************************************************************************************************/
#define I2C_DELAY                     10                                       // Microsecond delay on write
#define I2C_RESET_DELAY               28                                       // Microsecond delay for reset
#define INA_DEFAULT                 0x40                                       // Default address if only 1
#define INA_COUNT                      3                                       // Number of INA226s to monitor
#define INA_CONFIG_REG                 0                                       // Registers common to all INAs
#define INA_SHUNT_VOLT_REG             1                                       // Registers common to all INAs
#define INA_BUS_VOLT_REG               2                                       // Registers common to all INAs
#define INA_POWER_REGISTER             3                                       // Registers common to all INAs
#define INA_CURRENT_REGISTER           4                                       // Registers common to all INAs
#define INA_CALIBRATION_REG            5                                       // Registers common to all INAs
#define INA_MASK_ENABLE_REG            6                                       // Registers common to all INAs
#define INA_RESET_DEVICE          0x8000                                       // Write to configuration to reset
#define INA_DEFAULT_CONFIG        0x4127                                       // Default configuration register
#define INA_BUS_VOLT_LSB             125                                       // LSB in uV *100 1.25mV
#define INA_SHUNT_VOLT_LSB            25                                       // LSB in uV *10  2.5uV
#define INA_CONFIG_AVG_MASK       0x0E00                                       // Bits 9-11
#define INA_BUS_TIME_MASK         0x01C0                                       // Bits 6-8
#define INA_SHUNT_TIME_MASK       0x0038                                       // Bits 3-5
#define INA_CONV_RDY_MASK         0x0080                                       // Bit 4
#define INA_CONFIG_MODE_MASK      0x0007                                       // Bits 0-3
#define INA_TRIGGERED_SHUNT         B001                                       // Triggered shunt, no bus
#define INA_TRIGGERED_BUS           B010                                       // Triggered bus, no shunt
#define INA_TRIGGERED_BOTH          B011                                       // Triggered bus and shunt
#define INA_POWER_DOWN              B100                                       // shutdown or power-down
#define INA_CONTINUOUS_SHUNT        B101                                       // Continuous shunt, no bus
#define INA_CONTINUOUS_BUS          B110                                       // Continuous bus, no shunt
#define INA_CONTINUOUS_BOTH         B111                                       // Both continuous, default value

/*****************************************************************************************************************
** Declare structures used in the class                                                                         **
*****************************************************************************************************************/
typedef struct {                                                               // Structure of values per device
  uint8_t address;                                                             // I2C Address of device
  uint16_t calibration;                                                        // Calibration register value
  uint32_t current_LSB;                                                        // Amperage LSB
  uint32_t power_LSB;                                                          // Wattage LSB
  uint8_t operatingMode;                                                       // Mode operation
} inaSTRUCT; // of structure

/*****************************************************************************************************************
** Declare class header                                                                                         **
*****************************************************************************************************************/
class INA226_Class {                                                           // Class definition
  public:                                                                      // Publicly visible methods
    INA226_Class();                                                            // Class constructor
    ~INA226_Class();                                                           // Class destructor
    void begin(uint32_t MAXmilliAmps = 819,                                   // Class initializer (ebay special)
               uint32_t SenseRes = 100000,                                     // Current Sense in microOhms
               uint8_t  UNIT_ID = 0,                                           // INA226 Unit number
               uint8_t  I2C_ADDR = INA_DEFAULT);                               // INA226 I2C Address
    uint16_t getBusMilliVolts(bool waitSwitch = false,uint8_t UNIT_ID = 0);    // Retrieve Bus voltage in mV
    int16_t getShuntMicroVolts(bool waitSwitch = false,uint8_t UNIT_ID = 0);   // Retrieve Shunt voltage in uV
    int32_t getBusMicroAmps(uint8_t UNIT_ID = 0);                              // Retrieve micro-amps
    int32_t getBusMicroWatts(uint8_t UNIT_ID = 0);                             // Retrieve micro-watts
    uint8_t getAddress(uint8_t UNIT_ID = 0);                                   // Retrieve stored I2C Address
    uint16_t getCalibration(uint8_t UNIT_ID = 0);                              // Retrieve stored calibration
    uint32_t getCurrentLSB(uint8_t UNIT_ID = 0);                               // Retrieve stored current_LSB
    uint32_t getPowerLSB(uint8_t UNIT_ID = 0);                                 // Retrieve stored power_LSB
    uint8_t getOperatingMode(uint8_t UNIT_ID = 0);                             // Retrieve stored operatingMode
    void reset(uint8_t UNIT_ID = 0);                                           // Reset the device
    void setMode(uint8_t mode,uint8_t UNIT_ID = 0);                            // Set the monitoring mode
    void setAveraging(uint16_t averages,uint8_t UNIT_ID = 0);                  // Set the number of averages taken
    void setBusConversion(uint8_t convTime,uint8_t UNIT_ID = 0);               // Set timing for Bus conversions
    void setShuntConversion(uint8_t convTime,uint8_t UNIT_ID = 0);             // Set timing for Shunt conversions
    void waitForConversion(uint8_t UNIT_ID = 0);                               // wait for conversion to complete
    void setAlertPinOnConversion(bool alertState,uint8_t UNIT_ID = 0);         // Enable pin change on conversion
  private:                                                                     // Private variables and methods
    uint8_t inaReadByte(uint8_t regAddr,uint8_t I2C_ADDR);                     // Read a byte from an I2C address
    int16_t inaReadWord(uint8_t regAddr,uint8_t I2C_ADDR);                     // Read a word from an I2C address
    void inaWriteByte(uint8_t regAddr,uint8_t regData,uint8_t I2C_ADDR);       // Write a byte to an I2C address
    void inaWriteWord(uint8_t regAddr,uint16_t regData,uint8_t I2C_ADDR);      // Write two bytes to an I2C address
    uint8_t _TransmissionStatus = 0;                                           // Return code for I2C transmission
    inaSTRUCT inaARRAY[INA_COUNT];                                             // Build array to hold struct
}; // END of INA226_Class definition
#endif // END DBOE_INA226_h
