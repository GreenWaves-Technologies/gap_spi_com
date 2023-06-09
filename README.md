# USB SPI bridge to GAP9

This repository shows how to communicate over SPI between GAP9EVK and UMFT4222EV. 


## Physical Connection

The UMFT4222EV Dev board permits to power the FT4222 IOs from 1.8v to 5v. GAP9EVK IOs are at 1.8v, thus to avoid using level shifter **REMOVE J8 JUMPER from UMFT4222EV** and use GAP9EVK 1.8v to power FT4222 IOs. 


The following pin connections can be used to connect FT4222 BUS B with Gap9 SPI 2:

| UMFT4222EV         | Gap9EVK  | Description     |
|--------------------|----------|-----------------|
| JP5 PIN 1  - VCCIO | CN8 PIN 2| Gap  1.8V       |
| JP5 PIN 12 - GND   | CN6 PIN 8| Gap  GND        |
| JP4 PIN 9  - SS00  | CN6 PIN 7| Chip Select     |
| JP5 PIN 7  - MOSI  | CN6 PIN 6| MOSI - GAP SDI  |
| JP5 PIN 8  - MISO  | CN6 PIN 2| MISO - GAP SDO  |
| JP5 PIN 9  - SCK   | CN6 PIN 3| Serial Clock    |

### FT4222EV pinout

<div align="center">
<img src="resources/FT4222EV_pinout.png" width="30%" height="30%">
</div>


### GAP9EVK pinout

<div align="center">
<img src="resources/GAP9EVK_pinout.png" width="50%" height="50%">
</div>



## Project setup

The communication with FT4222 make use of a python library called python-ft4222 and some other libraries in python to install them:

   ```sh

   pip3 install -r requirements.txt
   ```

if you want to install python-ft4222 from source run: 

   ```sh
   ./setup.sh
   ```

## Notes

The documentation of UMFT4222EV can be found [here](https://ftdichip.com/wp-content/uploads/2020/07/DS_UMFT4222EV.pdf)


