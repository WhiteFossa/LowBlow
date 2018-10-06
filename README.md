# Project LowBlow - Advanced programmable cooler controller

Open source and hardware (GPLv3+) advanced programmable cooler controller.

## Features
- Automatically learns cooler characteristics: at power on it determines minimal voltage at one cooler starts, minimal RPMs and maximal RPMs, so you don't need to adjust temperature-to-RPMs settings for different coolers;
- Cooler speed is being controlled via RPMs loopback (using tachometer), so 50% RPMs will actually be 50% RPMs, not just a half of power voltage. In addition if anything affects cooler RPMs, voltage will be adjusted to keep desired RPMs.
- Control tool to configure temperature-to-RPMs curve. Using this tool you can set up curve via changing values in table, view how curve looks and finally generate EEPROM HEX file to flash it into controller.

Control tool screenshot:
![Control tool screenshot](https://raw.githubusercontent.com/WhiteFossa/LowBlow/master/ControlToolScreenshot.png)

## How to build/flash/use etc
Each project directory (i.e. Hardware, Firmware and Control Tool) contain README file with instructions.

## License
Everything in this project (except Intel HEX library) are distributed under GPLv3 or later version license.

Intel HEX library is written by  Kimmo Kulovesi, distributed under MIT license and can be downloaded from here: https://github.com/arkku/ihex
