#include "pmsis.h"
#include "multi_spi.h"

static uint8_t debug = 0;
static uint8_t *tx_buffer;

#define IMG_W  ( 320 )
#define IMG_H  ( 240 )
#define BUFFER_SIZE ( IMG_W * IMG_H )
#define CHUNK_SIZE_BITS (IMG_W << 3) // Get buffer size expressed in bits

#define MAGIC_NUMBER_H    0x47
#define MAGIC_NUMBER_L    0x41
#define CMD_STATUS_GET    0xf1
#define CMD_STATUS_SEND   0xf2
#define STATUS_RDY        0xD1


static void spi_slave_pad_init()
{
    pi_pad_function_set(SPI_SLAVE_PAD_SCK, SPI_SLAVE_PAD_FUNC);
    pi_pad_function_set(SPI_SLAVE_PAD_CS0, SPI_SLAVE_PAD_FUNC);
    pi_pad_function_set(SPI_SLAVE_PAD_SDO, SPI_SLAVE_PAD_FUNC);
    pi_pad_function_set(SPI_SLAVE_PAD_SDI, SPI_SLAVE_PAD_FUNC);
}

static void spi_slave_init(pi_device_t* spi_slave, struct pi_spi_conf* spi_slave_conf)
{
    pi_assert(spi_slave);
    pi_assert(spi_slave_conf);

    pi_spi_conf_init(spi_slave_conf);
    spi_slave_conf->wordsize = SPI_SLAVE_WORDSIZE;
    spi_slave_conf->big_endian = SPI_SLAVE_ENDIANESS;
    spi_slave_conf->max_baudrate = SPI_SLAVE_BAUDRATE;
    spi_slave_conf->polarity = SPI_SLAVE_POLARITY;
    spi_slave_conf->phase = SPI_SLAVE_PHASE;
    spi_slave_conf->itf = SPI_SLAVE_ITF;
    spi_slave_conf->cs = SPI_SLAVE_CS;
    spi_slave_conf->dummy_clk_cycle = SPI_MASTER_DUMMY_CYCLE;
    spi_slave_conf->dummy_clk_cycle_mode = SPI_MASTER_DUMMY_CYCLE_MODE;
    spi_slave_conf->is_slave = SPI_SLAVE_IS_SLAVE;
    pi_open_from_conf(spi_slave, spi_slave_conf);
}

// Test *******************************************************************************************

static void receive_callback(void* context)
{

    if(debug) printf("INFO: Slave SPI receive is finished\n");
    pi_evt_push((pi_evt_t*)context);
}

static void send_callback(void* context)
{
    if(debug) printf("INFO: Master SPI send is finished\n");
    pi_evt_push((pi_evt_t*)context);
}


int main(void)
{
    printf("\n\t *** PMSIS Multi SPI dummy clock cycles example ***\n\n");
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, 370*1000*1000);
    pi_freq_set(PI_FREQ_DOMAIN_FC,     370*1000*1000);
     
    int retval = 0;
    pi_device_t spi_slave;
    struct pi_spi_conf spi_slave_conf;
    pi_evt_t send_task, receive_task, end_task;

    // Initialize Gap9 EVK GPIOs
    spi_slave_pad_init();

    // Initialize SPI
    spi_slave_init(&spi_slave, &spi_slave_conf);
    if (pi_spi_open(&spi_slave))
    {
        printf("ERROR: Failed to open SPI peripheral\n");
        return -1;
    }

    tx_buffer  = pi_l2_malloc(BUFFER_SIZE);

    int iter=0;
    while (1)
    {
        //Create a syntethic image and change it every frame to evaluate framerate
        for(int i=0;i<IMG_H;i++)
            for(int j=0;j<IMG_W;j++)
                tx_buffer[i*IMG_W+j]=(iter%2)?(i):(255-i);
        iter++;

        pi_spi_receive(&spi_slave, tx_buffer, 3 << 3, SPI_NO_OPTION);
        if(tx_buffer[0]!=MAGIC_NUMBER_H && tx_buffer[1]!=MAGIC_NUMBER_L && tx_buffer[2]!=  CMD_STATUS_GET)
            continue;
        tx_buffer[0]= MAGIC_NUMBER_H;
        tx_buffer[1]= MAGIC_NUMBER_L;
        tx_buffer[2]= STATUS_RDY;
        pi_spi_send(&spi_slave, tx_buffer, 3 << 3, SPI_NO_OPTION);
        uint16_t* img_size = tx_buffer;
        img_size[0] = IMG_H;
        img_size[1] = IMG_W;
        pi_spi_send(&spi_slave, img_size,   2 << 3, SPI_NO_OPTION);
        pi_spi_send(&spi_slave, img_size+1, 2 << 3, SPI_NO_OPTION);
        
        for(int i=0;i<IMG_H;i++){
            pi_evt_sig_init(&end_task);
            pi_spi_send_async(&spi_slave, tx_buffer+(i*IMG_W), (IMG_W) << 3, SPI_NO_OPTION, pi_evt_callback_irq_init(&send_task, &send_callback, &end_task));
            pi_evt_wait(&end_task);
        }
    }

    pi_l2_free(tx_buffer, BUFFER_SIZE);
    printf("\n\t *** End of test ***\n\n");
    return retval;
}
