# INA226
<img src="https://github.com/dboe/DBOE_INA226/blob/master/Images/INA226.jpg" width="175" align="right"/>INA226 High-Side/Low-Side Bi-Directional I2C Current and Power Monitor library for the Arduino.  Texas Instruments produces this family of power monitors and the series is described at on their product page at [INA226](http://www.ti.com/product/INA226).
## Hardware layout
The [datasheet](http://www.ti.com/lit/ds/symlink/ina226.pdf) has examples of how to hook up INA226.
## Library description
The library uses INA226 devices connected to the I2C chain. Each unit can be individually configured with 4 setup parameters describing the expected voltage, shunt / resistor values which then set the internal configuration registers is ready to begin accurate measurements.  The details of how to setup the library along with all of the publicly available methods can be found on the [INA226 wiki pages](https://github.com/dboe/DBOE_INA226/wiki).

The INA226 has a dedicated interrupt pin which can be used to trigger pin-change interrupts on the Arduino and the examples contain a program that measures readings using this output pin so that the Arduino can perform other concurrent tasks while still retrieving measurements.

![dboe Logo](https://www.boechler.net/images/site/gif/.gif) <img src="https://www.boechler.net/images/site/gif/gnomishworks2_400.jpg" width="75"/>
