# FT4222 SPI communication with Gap9

This repository shows how to comunication between Gap9 and UMFT4222EV


## Physical Connection

The UMFT4222EV board can be powered at 1.8V removing J8 and connecting. Thus there is no need for level shifters.


UMFT4222EV PIN NAME| UMFT4222EV | Gap9EVK  | Description     |
|------------------|------------|----------|-----------------|
| VCCIO            |JP5 PIN 1   | CN8 PIN 2| Gap  1.8V       |
| GND              |JP5 PIN 12  | CN8 PIN 4| Gap  GND        |
| SS00             |JP4 PIN 9   | CN6 PIN 7| Chip Select     |
| SCK              |JP5 PIN 9   | CN6 PIN 3| SClock          |
| MOSI             |JP4 PIN 6   | CN6 PIN 7| MOSI - GAP SDI  |
| MISO             |JP4 PIN 2   | CN6 PIN 8| MISO - GAP SDO  |

![UMFT4222EV](resources/FT4222EV_pinout.png)




Doc of UMFT4222EV can be found [here](https://ftdichip.com/wp-content/uploads/2020/07/DS_UMFT4222EV.pdf)


