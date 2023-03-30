import ft4222
from ft4222.SPI import Cpha, Cpol
from ft4222.SPIMaster import Mode, Clock, SlaveSelect
from ft4222.GPIO import Port, Dir
from time import sleep
import sys
from timeit import default_timer as timer


nbDev = ft4222.createDeviceInfoList()
print("nb of fdti devices: {}".format(nbDev))

ftDetails = []

if nbDev <= 0:
    print("no devices found...")
    sys.exit(0)

print("devices:")
for i in range(nbDev):
    detail = ft4222.getDeviceInfoDetail(i, False)
    print(" - {}".format(detail))
    ftDetails.append(detail)



# open 'device' with default description 'FT4222 A'
devA = ft4222.openByDescription('FT4222')
# and the second 'device' on the same chip
#devB = ft4222.openByDescription('FT4222 B')

# init spi master
""" 
    Attributes:
        NONE:
        DIV_2: 1/2 System Clock
        DIV_4: 1/4 System Clock
        DIV_8: 1/8 System Clock
        DIV_16: 1/16 System Clock
        DIV_32: 1/32 System Clock
        DIV_64: 1/64 System Clock
        DIV_128: 1/128 System Clock
        DIV_256: 1/256 System Clock
        DIV_512: 1/512 System Clock

    NONE    = 0 // 60000000 Hz
    DIV_2   = 1 // 30000000 Hz
    DIV_4   = 2 // 15000000 Hz
    DIV_8   = 3 //  7500000 Hz
    DIV_16  = 4 //  3750000 Hz
    DIV_32  = 5 //  1875000 Hz
    DIV_64  = 6 //   937500 Hz
    DIV_128 = 7
    DIV_256 = 8
    DIV_512 = 9

Cpha.CLK_LEADING  Cpha.CLK_TRAILING
Cpol.IDLE_LOW, Cpol.IDLE_HIGH

"""

# Clock on UMFT4222EV is at 60 MhZ 
devA.spiMaster_Init(Mode.SINGLE, Clock.DIV_4,  Cpha.CLK_LEADING, Cpol.IDLE_LOW, SlaveSelect.SS0)

# generate data to send
data = bytes([x for x in range(0,15)])

data=b'\x00\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f'

# set port0 1 (-> note this is *not* the spi chip select, the chip select (SS0) is generated by the spi core)
#devB.gpio_Write(Port.P0, 1)

start = timer()

while len(data)<320*240:
    # write data in a single write
    #devA.spiMaster_SingleWrite(data, True)
    data+= devA.spiMaster_SingleRead(321,True)
    #print(data)
    # wait a short while
    #sleep(0.5)

end = timer()

print(data)
print(end - start)
# set port0 0
#devB.gpio_Write(Port.P0, 1)