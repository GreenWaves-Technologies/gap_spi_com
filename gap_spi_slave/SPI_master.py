import ft4222
from ft4222.SPI import Cpha, Cpol
from ft4222.SPIMaster import Mode, Clock, SlaveSelect
from ft4222.GPIO import Port, Dir
from time import sleep
import sys
from timeit import default_timer as timer
import PIL.Image as Image
import cv2
import numpy

IMG_W=320
IMG_H=240

magic_number    = b'\x47\x41'
magic_number_h  = 0x47
magic_number_l  = 0x41
cmd_status_get  = b'\xF1'
cmd_status_send = b'\xF2'
status_ready    = 0xD1
pad_value       = b'\x00'


def main():
    nbDev = ft4222.createDeviceInfoList()
    print("nb of fdti devices: {}".format(nbDev))

    ftDetails = []

    if nbDev <= 0:
        print("no devices found...")
        return

    print("devices:")
    for i in range(nbDev):
        detail = ft4222.getDeviceInfoDetail(i, False)
        print(" - {}".format(detail))
        ftDetails.append(detail)



    # open 'device' with default description 'FT4222'
    devA = ft4222.openByDescription('FT4222')

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

    # set port0 1 (-> note this is *not* the spi chip select, the chip select (SS0) is generated by the spi core)
    #devB.gpio_Write(Port.P0, 1)
    data_s = bytearray()
    data_r = bytearray()

    
    while True:
        data_s = magic_number + cmd_status_get
        img_data   = bytearray()    
        #Inquiry device to see if it is ready to send data
        devA.spiMaster_SingleWrite(pad_value + data_s,True)
        data_r =devA.spiMaster_SingleRead(3+1,True)

        #print(hex(data_r[1]),hex(data_r[2]),hex(data_r[3]))

        if data_r[1] == magic_number_h and data_r[2] == magic_number_l and data_r[3] == status_ready:
            #read image height
            data_r =devA.spiMaster_SingleRead(2+1,True)
            im_hight = (data_r[2] << 8) + data_r[1]
            #read image width
            data_r =devA.spiMaster_SingleRead(2+1,True)
            im_width = (data_r[2] << 8) + data_r[1] 
            start = timer()
            
            for x in range(0, im_hight):
                tmp_data  =devA.spiMaster_SingleRead(im_width+1,True)
                img_data +=tmp_data[1:im_width+1]
                #print(len(tmp_data),len(img_data))
                # wait a short while
                #sleep(0.5)
            
            end = timer()
            
            #im = Image.frombuffer('L',(IMG_W,IMG_H),data,'raw','L',0,1)
            #im = Image.open(data)
            flatNumpyArray = numpy.array(img_data,dtype=numpy.uint8)

            open_cv_image = flatNumpyArray.reshape(im_hight, im_width)
            
            cv2.imshow('Gap Output',open_cv_image)
            cv2.waitKey(1)
            
            ttime=(end - start)
            fps=1/ttime
            print('FPS: ',round(fps,2),'\t Bandwidth: ',round(((IMG_W*IMG_H)/ttime)/1000000,3),'MBytes/s')


if __name__ == "__main__":
    sys.exit(main())