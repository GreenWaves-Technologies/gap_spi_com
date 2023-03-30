# FT4222 SPI communication with GAP9

This repository shows how to comunication between Gap9 and UMFT4222EV


## Physical Connection

The UMFT4222EV board can be powered at 1.8V from GAP9EVK **removing J8 Jumper**. Thus there is no need for level shifters.


The following pin connections can be used to connect FT4222 BUS B with Gap9 SPI 2:

<div align="center">
| UMFT4222EV         | Gap9EVK  | Description     |
|--------------------|----------|-----------------|
| JP5 PIN 1  - VCCIO | CN8 PIN 2| Gap  1.8V       |
| JP5 PIN 12 - GND   | CN8 PIN 4| Gap  GND        |
| JP4 PIN 9  - SS00  | CN6 PIN 7| Chip Select     |
| JP4 PIN 7  - MOSI  | CN6 PIN 6| MOSI - GAP SDI  |
| JP4 PIN 8  - MISO  | CN6 PIN 2| MISO - GAP SDO  |
| JP5 PIN 9  - SCK   | CN6 PIN 3| Serial Clock    |
</div>


### FT4222EV pinout

<div align="center">
<img src="resources/FT4222EV_pinout.png " width="50%" height="50%">
</div>
## Project setup

The communication with FT4222 make use of a python library called python-ft4222 to download from github and set it up:

   ```sh
   ./setup.sh
   ```


The documentation of UMFT4222EV can be found [here](https://ftdichip.com/wp-content/uploads/2020/07/DS_UMFT4222EV.pdf)


