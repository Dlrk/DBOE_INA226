/*******************************************************************************************************************
** INA class method definitions for INA226 Library.                                                               **
**                                                                                                                **
** See the INA226.h header file comments for version information.                                                 **
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
#ifdef __AVR__
  #include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
 #include <pgmspace.h>
#else
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif

#if !defined(__ARM_ARCH) && !defined(ENERGIA) && !defined(ESP8266) && !defined(ESP32) && !defined(__arc__)
 #include <util/delay.h>
#endif

#include <stdlib.h>
#include "DBOE_INA226.h"                                                       // Include the header definition
#include <Wire.h>                                                              // I2C Library definition
INA226_Class::INA226_Class()  {}                                               // Class constructor
INA226_Class::~INA226_Class() {}                                               // Unused class destructor

/*******************************************************************************************************************
** Method begin() sets the INA226 Configuration details, If no details a single device (ebay special) is assumed  **
*******************************************************************************************************************/
void INA226_Class::begin(uint8_t MAXmilliAmps,                                 // Max expected milliamps
                         uint32_t CurrentSenseR,                               // Current Sense in microOhms
                         uint8_t UNIT_ID,                                      // INA226 Unit number
                         uint8_t I2C_ADDR) {                                   // INA226 I2C Address
  Wire.begin();                                                                // Start the I2C wire subsystem
  Wire.beginTransmission(I2C_ADDR);                                            // See if something is at address
  if (Wire.endTransmission() == 0) {                                           // by checking the return error
    inaWriteWord(INA_CONFIG_REG,INA_RESET_DEVICE,I2C_ADDR);                    // Force INAs to reset
    delayMicroseconds(I2C_RESET_DELAY);                                        // Wait for INA to finish resetting
    if (inaReadWord(INA_CONFIG_REG,I2C_ADDR)==INA_DEFAULT_CONFIG) {            // Yes, we've found a INA226!
      inaARRAY[UNIT_ID].address = I2C_ADDR;                                    // Store device address
      inaARRAY[UNIT_ID].current_LSB =  (uint64_t)MAXmilliAmps*1000000/32767;   // Get the best possible LSB in nA
      inaARRAY[UNIT_ID].calibration = (uint64_t)51200000 /                     // Compute calibration register
        ((uint64_t)inaARRAY[UNIT_ID].current_LSB *
        (uint64_t)CurrentSenseR / (uint64_t)100000);
      inaARRAY[UNIT_ID].power_LSB = inaARRAY[UNIT_ID].current_LSB * 25;        // Fixed multiplier for INA226
      inaARRAY[UNIT_ID].operatingMode = B111;                                  // Default to continuous mode
      inaWriteWord(INA_CALIBRATION_REG,inaARRAY[UNIT_ID].calibration,          // Write the calibration value
                inaARRAY[UNIT_ID].address);                                    // INA226 I2C Address
    }
  }
} // END of method begin()

/*******************************************************************************************************************
** Method inaReadByte reads 1 byte from the specified address                                                     **
*******************************************************************************************************************/
uint8_t INA226_Class::inaReadByte(uint8_t regAddr,uint8_t deviceAddr) {
  Wire.beginTransmission(deviceAddr);                                          // INA226 I2C Address
  WIRE_WRITE(regAddr);                                                         // Send the register address to read
  _TransmissionStatus = Wire.endTransmission();                                // Close transmission
  delayMicroseconds(I2C_DELAY);                                                // delay required for sync
  Wire.requestFrom(deviceAddr, (uint8_t)1);                                    // Request 1 byte of data
  return WIRE_READ();                                                          // read it and return it
} // END of method inaReadByte()

/*******************************************************************************************************************
** Method inaReadWord reads 2 bytes from the specified address                                                    **
*******************************************************************************************************************/
int16_t INA226_Class::inaReadWord(uint8_t regAddr,uint8_t deviceAddr) {
  int16_t returnData;                                                          // Store return value
  Wire.beginTransmission(deviceAddr);                                          // INA226 I2C Address
  WIRE_WRITE(regAddr);                                                         // Send the register address to read
  _TransmissionStatus = Wire.endTransmission();                                // Close transmission
  delayMicroseconds(I2C_DELAY);                                                // delay required for sync
  Wire.requestFrom(deviceAddr, (uint8_t)2);                                    // Request 2 consecutive bytes
  returnData = WIRE_READ();                                                    // Read the msb
  returnData = returnData<<8;                                                  // shift the data over
  returnData|= WIRE_READ();                                                    // Read the lsb
  return returnData;                                                           // read it and return it
} // END of method inaReadWord()

/*******************************************************************************************************************
** Method inaWriteByte write 1 byte to the specified address                                                      **
*******************************************************************************************************************/
void INA226_Class::inaWriteByte(uint8_t regAddr,uint8_t regData,uint8_t deviceAddr) {
  Wire.beginTransmission(deviceAddr);                                          // INA226 I2C Address
  WIRE_WRITE(regAddr);                                                         // Send register address to write
  WIRE_WRITE(regData);                                                         // Send the data to write
  _TransmissionStatus = Wire.endTransmission();                                // Close transmission
} // END of method inaWriteByte()

/*******************************************************************************************************************
** Method inaWriteWord writes 2 byte to the specified address                                                     **
*******************************************************************************************************************/
void INA226_Class::inaWriteWord(uint8_t regAddr, uint16_t regData,uint8_t deviceAddr) {
  Wire.beginTransmission(deviceAddr);                                          // INA226 I2C Address
  WIRE_WRITE(regAddr);                                                         // Send register address to write
  WIRE_WRITE((uint8_t)(regData>>8));                                           // Write the first byte
  WIRE_WRITE((uint8_t)regData);                                                // and then the second
  _TransmissionStatus = Wire.endTransmission();                                // Close transmission
} // END of method inaWriteWord()

/*******************************************************************************************************************
** Method getBusMilliVolts retrieves the bus voltage measurement                                                  **
*******************************************************************************************************************/
uint16_t INA226_Class::getBusMilliVolts(bool waitSwitch,uint8_t UNIT_ID) {
  if (waitSwitch) waitForConversion();                                         // wait for conversion to complete
  uint16_t busVoltage = inaReadWord(INA_BUS_VOLT_REG,inaARRAY[UNIT_ID].address);// Get the raw value and apply
  busVoltage = (uint32_t)busVoltage*INA_BUS_VOLT_LSB/100;                      // conversion to get milliVolts
  if (!bitRead(inaARRAY[UNIT_ID].operatingMode,2) &&                           // If triggered mode and bus active
       bitRead(inaARRAY[UNIT_ID].operatingMode,1)) {
    int16_t configRegister = inaReadWord(INA_CONFIG_REG,inaARRAY[UNIT_ID].address);// Get the current register
    inaWriteWord(INA_CONFIG_REG,configRegister,inaARRAY[UNIT_ID].address);     // Write back to trigger next
  }
  return(busVoltage);                                                          // return computed milliVolts
} // END of method getBusMilliVolts()

/*******************************************************************************************************************
** Method getShuntMicroVolts retrieves the shunt voltage measurement                                              **
*******************************************************************************************************************/
int16_t INA226_Class::getShuntMicroVolts(bool waitSwitch,uint8_t UNIT_ID) {
  if (waitSwitch) waitForConversion();                                         // wait for conversion to complete
  int32_t shuntVoltage = inaReadWord(INA_SHUNT_VOLT_REG,inaARRAY[UNIT_ID].address);// Get the raw value
  shuntVoltage = shuntVoltage*INA_SHUNT_VOLT_LSB/10;                           // Convert to microvolts
  if (!bitRead(inaARRAY[UNIT_ID].operatingMode,2) &&                           // If triggered mode and bus active
       bitRead(inaARRAY[UNIT_ID].operatingMode,1)) {
    int16_t configRegister = inaReadWord(INA_CONFIG_REG,inaARRAY[UNIT_ID].address);// Get the current register
    inaWriteWord(INA_CONFIG_REG,configRegister,inaARRAY[UNIT_ID].address);     // Write back to trigger next
  }
  return((int16_t)shuntVoltage);                                               // return computed microvolts
} // END of method getShuntMicroVolts()

/*******************************************************************************************************************
** Method getBusMicroAmps retrieves the computed current in microamps.                                            **
*******************************************************************************************************************/
int32_t INA226_Class::getBusMicroAmps(uint8_t UNIT_ID) {
  int32_t microAmps = inaReadWord(INA_CURRENT_REGISTER,inaARRAY[UNIT_ID].address);// Get the raw value
          microAmps = (int64_t)microAmps*inaARRAY[UNIT_ID].current_LSB/1000;   // Convert to microamps
  return(microAmps);                                                           // return computed microamps
} // END of method getBusMicroAmps()

/*******************************************************************************************************************
** Method getBusMicroWatts retrieves the computed power in milliwatts                                             **
*******************************************************************************************************************/
int32_t INA226_Class::getBusMicroWatts(uint8_t UNIT_ID) {
  int32_t microWatts = inaReadWord(INA_POWER_REGISTER,inaARRAY[UNIT_ID].address);// Get the raw value
          microWatts = (int64_t)microWatts*inaARRAY[UNIT_ID].power_LSB/1000;   // Convert to milliwatts
  return(microWatts);                                                          // return computed milliwatts
} // END of method getBusMicroWatts()

/*******************************************************************************************************************
** Method reset resets the INA226 using the first bit in the configuration register                               **
*******************************************************************************************************************/
void INA226_Class::reset(uint8_t UNIT_ID) {                                    // Reset the INA226
  inaWriteWord(INA_CONFIG_REG,0x8000,inaARRAY[UNIT_ID].address);               // Set most significant bit
  delayMicroseconds(I2C_RESET_DELAY);                                          // Let the INA226
} // END of method reset

/*******************************************************************************************************************
** Method setMode allows the various mode combinations to be set. If no parameter is given the system goes back   **
** to the default startup mode.                                                                                   **
*******************************************************************************************************************/
void INA226_Class::setMode(uint8_t mode,uint8_t UNIT_ID ) {                    // Set the monitoring mode
  int16_t configRegister;                                                      // Hold configuration register
  configRegister = inaReadWord(INA_CONFIG_REG,inaARRAY[UNIT_ID].address);      // Get the current register
  configRegister &= ~INA_CONFIG_MODE_MASK;                                     // zero out the mode bits
  inaARRAY[UNIT_ID].operatingMode = B00001111 & mode;                          // Mask off unused bits
   configRegister |= inaARRAY[UNIT_ID].operatingMode;                          // shift in the mode settings
  inaWriteWord(INA_CONFIG_REG,configRegister,inaARRAY[UNIT_ID].address);       // Save new value
} // END of method setMode()

/*******************************************************************************************************************
** Method setAveraging sets the hardware averaging for the different devices                                      **
*******************************************************************************************************************/
void INA226_Class::setAveraging(uint16_t averages,uint8_t UNIT_ID ) {          // Set the number of averages taken
  uint8_t averageIndex;                                                        // Store indexed value for register
  int16_t configRegister;                                                      // Configuration register contents
  configRegister = inaReadWord(INA_CONFIG_REG,inaARRAY[UNIT_ID].address);      // Get the current register
  if      (averages>=1024) averageIndex = 7;                                   // setting depending upon range
  else if (averages>= 512) averageIndex = 6;
  else if (averages>= 256) averageIndex = 5;
  else if (averages>= 128) averageIndex = 4;
  else if (averages>=  64) averageIndex = 3;
  else if (averages>=  16) averageIndex = 2;
  else if (averages>=   4) averageIndex = 1;
  else                     averageIndex = 0;
  configRegister &= ~INA_CONFIG_AVG_MASK;                                      // zero out the averages part
  configRegister |= (uint16_t)averageIndex << 9;                               // shift in the averages to register
  inaWriteWord(INA_CONFIG_REG,configRegister,inaARRAY[UNIT_ID].address);       // Save new value
} // END of method setAveraging()

/*******************************************************************************************************************
** Method setBusConversion specifies the conversion rate (see datasheet for 8 distinct values) for the bus        **
*******************************************************************************************************************/
void INA226_Class::setBusConversion(uint8_t convTime,uint8_t UNIT_ID ) {       // Set timing for Bus conversions
  int16_t configRegister;                                                      // Store configuration register
  if (convTime>7) convTime=7;                                                  // Use maximum value allowed
  configRegister = inaReadWord(INA_CONFIG_REG,inaARRAY[UNIT_ID].address);      // Get the current register
  configRegister &= ~INA_BUS_TIME_MASK;                                        // zero out the Bus conversion part
  configRegister |= (uint16_t)convTime << 6;                                   // shift in the averages to register
  inaWriteWord(INA_CONFIG_REG,configRegister,inaARRAY[UNIT_ID].address);       // Save new value
} // END of method setBusConversion()

/*******************************************************************************************************************
** Method setShuntConversion specifies the conversion rate (see datasheet for 8 distinct values) for the shunt    **
*******************************************************************************************************************/
void INA226_Class::setShuntConversion(uint8_t convTime,uint8_t UNIT_ID ) {     // Set timing for Bus conversions
  int16_t configRegister;                                                      // Store configuration register
  if (convTime>7) convTime=7;                                                  // Use maximum value allowed
  configRegister = inaReadWord(INA_CONFIG_REG,inaARRAY[UNIT_ID].address);      // Get the current register
  configRegister &= ~INA_SHUNT_TIME_MASK;                                      // zero out the Bus conversion part
  configRegister |= (uint16_t)convTime << 3;                                   // shift in the averages to register
  inaWriteWord(INA_CONFIG_REG,configRegister,inaARRAY[UNIT_ID].address);       // Save new value
} // END of method setShuntConversion()

/*******************************************************************************************************************
** Method waitForConversion loops until the current conversion is marked as finished. If the conversion has       **
** completed already then the flag (and interrupt pin, if activated) is also reset.                               **
*******************************************************************************************************************/
void INA226_Class::waitForConversion(uint8_t UNIT_ID) {                        // Wait for current conversion
  uint16_t conversionBits = 0;
  while(conversionBits==0) {
    conversionBits = inaReadWord(INA_MASK_ENABLE_REG,inaARRAY[UNIT_ID].address) &(uint16_t)8;
  }
} // END of method waitForConversion()

/*******************************************************************************************************************
** Method setAlertPinOnConversion configure the INA226 to pull the ALERT pin low when a conversion is complete    **
*******************************************************************************************************************/
void INA226_Class::setAlertPinOnConversion(bool alertState,uint8_t UNIT_ID ) { // Enable pin change on conversion
  uint16_t alertRegister;                                                      // Hold the alert register
  alertRegister = inaReadWord(INA_MASK_ENABLE_REG,inaARRAY[UNIT_ID].address);  // Get the current register
  if (!alertState) alertRegister &= ~((uint16_t)1<<10);                        // zero out the alert bit
  else alertRegister |= (uint16_t)(1<<10);                                     // turn on the alert bit
  inaWriteWord(INA_MASK_ENABLE_REG,alertRegister,inaARRAY[UNIT_ID].address);   // Write register back to device
} // END of method setAlertPinOnConversion
