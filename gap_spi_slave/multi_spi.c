#include "pmsis.h"

// Spi Master configuration
#define SPI_MASTER_BAUDRATE             1000000
#define SPI_MASTER_WORDSIZE             PI_SPI_WORDSIZE_8
#define SPI_MASTER_ENDIANESS            1
#define SPI_MASTER_POLARITY             PI_SPI_POLARITY_0
#define SPI_MASTER_PHASE                PI_SPI_PHASE_0
#define SPI_MASTER_CS                   1                                   // CSO (GPIO34)
#define SPI_MASTER_ITF                  1                                   // SPI1 peripheral is set as master
#define SPI_MASTER_DUMMY_CYCLE          3
#define SPI_MASTER_DUMMY_CYCLE_MODE     PI_SPI_DUMMY_CLK_CYCLE_BEFORE_CS

// Spi Master pad configuration
#define SPI_MASTER_PAD_SCK              PI_PAD_033  // Ball B2          > CN2 connector, pin 5
#define SPI_MASTER_PAD_CS0              PI_PAD_034  // Ball E4          > CN2 connector, pin 4
#define SPI_MASTER_PAD_SDO              PI_PAD_038  // Ball A1  (MOSI)  > CN2 connector, pin 1
#define SPI_MASTER_PAD_SDI              PI_PAD_039  // Ball E3  (MISO)  > CN3 connector, pin 5
#define SPI_MASTER_PAD_CS1              PI_PAD_067  // Ball E4          > CN2 connector, pin 4
#define SPI_MASTER_PAD_FUNC             PI_PAD_FUNC0
#define SPI_MASTER_IS_SLAVE             0

// SPI Slave Configuration
#define SPI_SLAVE_BAUDRATE              15000000
#define SPI_SLAVE_WORDSIZE              PI_SPI_WORDSIZE_8
#define SPI_SLAVE_ENDIANESS             1
#define SPI_SLAVE_POLARITY              PI_SPI_POLARITY_1
#define SPI_SLAVE_PHASE                 PI_SPI_PHASE_1
#define SPI_SLAVE_CS                    0                   // (GPIO57) Use SPI2 CS0 on GAP9EVK's CN6(7) connector
#define SPI_SLAVE_ITF                   2                   // SPI2 peripheral is set as slave
#define SPI_SLAVE_IS_SLAVE              1

// Spi slave pad configuration
#define SPI_SLAVE_PAD_SCK               PI_PAD_056  // Ball H1  > GAP9 EVK CN6 connector, pin 3
#define SPI_SLAVE_PAD_CS0               PI_PAD_057  // Ball H2  > GAP9 EVK CN6 connector, pin 7
#define SPI_SLAVE_PAD_CS1               PI_PAD_053  // Ball H7  > GAP9 EVK CN5 connector, pin 10
#define SPI_SLAVE_PAD_SDO               PI_PAD_059  // Ball H4  > GAP9 EVK CN6 connector, pin 2
#define SPI_SLAVE_PAD_SDI               PI_PAD_058  // Ball H3  > GAP9 EVK CN6 connector, pin 6
#define SPI_SLAVE_PAD_FUNC              PI_PAD_FUNC2

#define SPI_NO_OPTION                   0 // No option applied for SPI transfer
// Data size to send
#define CHUNK_SIZE  ( 320 )
#define CHUNKS      ( 240 )
#define CHUNK_SIZE_BITS (CHUNK_SIZE << 3) // Get buffer size expressed in bits
#define BUFFER_SIZE ( CHUNK_SIZE * CHUNKS )

static uint8_t debug = 0;
static uint8_t *spi_master_tx_buf;
static uint8_t *spi_master_rx_buf;
static uint8_t *spi_slave_rx_buf;

// GPIO Initialization ****************************************************************************

static void spi_master_pad_init()
{
    pi_pad_function_set(SPI_MASTER_PAD_SCK, SPI_MASTER_PAD_FUNC);
    pi_pad_function_set(SPI_MASTER_PAD_SDO, SPI_MASTER_PAD_FUNC);
    pi_pad_function_set(SPI_MASTER_PAD_SDI, SPI_MASTER_PAD_FUNC);
    pi_pad_mux_group_set(SPI_MASTER_PAD_CS1, PI_PAD_MUX_GROUP_SPI1_CS1);
    pi_pad_function_set(SPI_MASTER_PAD_CS1, SPI_MASTER_PAD_FUNC);
    pi_pad_function_set(SPI_MASTER_PAD_CS0, SPI_MASTER_PAD_FUNC);
}

static void spi_slave_pad_init()
{
    pi_pad_function_set(SPI_SLAVE_PAD_SCK, SPI_SLAVE_PAD_FUNC);
    pi_pad_function_set(SPI_SLAVE_PAD_CS0, SPI_SLAVE_PAD_FUNC);
    pi_pad_function_set(SPI_SLAVE_PAD_SDO, SPI_SLAVE_PAD_FUNC);
    pi_pad_function_set(SPI_SLAVE_PAD_SDI, SPI_SLAVE_PAD_FUNC);
}

// SPI Initialization *****************************************************************************

// static void spi_master_init(pi_device_t* spi_master, struct pi_spi_conf* spi_master_conf)
// {
//     pi_assert(spi_master);
//     pi_assert(spi_master_conf);

//     pi_spi_conf_init(spi_master_conf);
//     spi_master_conf->wordsize = SPI_MASTER_WORDSIZE;
//     spi_master_conf->big_endian = SPI_MASTER_ENDIANESS;
//     spi_master_conf->max_baudrate = SPI_MASTER_BAUDRATE;
//     spi_master_conf->polarity = SPI_MASTER_POLARITY;
//     spi_master_conf->phase = SPI_MASTER_PHASE;
//     spi_master_conf->itf = SPI_MASTER_ITF;
//     spi_master_conf->cs = SPI_MASTER_CS;
//     spi_master_conf->dummy_clk_cycle = SPI_MASTER_DUMMY_CYCLE;
//     spi_master_conf->dummy_clk_cycle_mode = SPI_MASTER_DUMMY_CYCLE_MODE;
//     spi_master_conf->is_slave = SPI_MASTER_IS_SLAVE;
//     pi_open_from_conf(spi_master, spi_master_conf);

//     printf("INFO: Dummy clock cycle number : %d\n", spi_master_conf->dummy_clk_cycle);
//     printf("INFO: Dummy clock cycle mode   : %d\n", spi_master_conf->dummy_clk_cycle_mode);
// }

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

/**
 * @brief   Slave SPI callback on receiving data.
 *          The context is a notification that block the execution
 *          of the test until the SPI slave receive step is over.
 * @param context End notification.
 */
static void receive_callback(void* context)
{

    //if(debug) 
    printf("INFO: Slave SPI receive is finished\n");
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
    pi_freq_set(PI_FREQ_DOMAIN_FC, 370*1000*1000);
    int retval = 0;
    pi_device_t spi_master, spi_slave;
    struct pi_spi_conf spi_master_conf, spi_slave_conf;
    pi_evt_t send_task, receive_task, end_task;

    // Initialize Gap9 EVK GPIOs
    //spi_master_pad_init();
    spi_slave_pad_init();

    // Initialize SPIs
    //spi_master_init(&spi_master, &spi_master_conf);
    spi_slave_init(&spi_slave, &spi_slave_conf);
    //if (pi_spi_open(&spi_master) || pi_spi_open(&spi_slave))
    if (pi_spi_open(&spi_slave))
    {
        printf("ERROR: Failed to open SPI peripheral\n");
        retval = -1;
    }
    else
    {
        //spi_master_tx_buf = pi_l2_malloc(BUFFER_SIZE);
        //spi_master_rx_buf = pi_l2_malloc(BUFFER_SIZE);
        spi_slave_rx_buf  = pi_l2_malloc(BUFFER_SIZE);

        // Prepare data buffer to send
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            //spi_master_tx_buf[i] = BUFFER_SIZE - i;
            //spi_master_rx_buf[i] = 0;
            spi_slave_rx_buf[i] = i%320;
        }

        // for (int i = 0; i < CHUNKS; i++)
        // {
        //     // Initialize a notification event to block this
        //     // process until spi master send and spi slave receive
        //     // operation are performed.
        //     pi_evt_sig_init(&end_task);

        //     // Initiate SPI slave receive
        //     pi_spi_receive_async(&spi_slave, (spi_slave_rx_buf + (i*CHUNK_SIZE)), CHUNK_SIZE_BITS, SPI_NO_OPTION, pi_evt_callback_irq_init(&receive_task, &receive_callback, (void*)&end_task));

        //     // Initiate SPI master send
        //     //pi_spi_send_async(&spi_master, (spi_master_tx_buf + (i*CHUNK_SIZE)), CHUNK_SIZE_BITS, SPI_NO_OPTION, pi_evt_callback_irq_init(&send_task, &send_callback, NULL));

        //     // Blocking the execution until send and receive steps are done.
        //     pi_evt_wait(&end_task);

        //     printf("Received somenthing:\n");
        //     for(int a=0;a<CHUNK_SIZE;a+=4){
        //         printf("0x%x 0x%x 0x%x 0x%x\n",spi_slave_rx_buf[i*CHUNK_SIZE+a+0],spi_slave_rx_buf[i*CHUNK_SIZE+a+1],spi_slave_rx_buf[i*CHUNK_SIZE+a+2],spi_slave_rx_buf[i*CHUNK_SIZE+a+3]);
        //     }

            
        // }

        printf("master -> slave test done\n");

        for (int i = 0; i < CHUNKS; i++)
        {
            // Initialize a notification event to block this
            // process until spi master receive and spi slave sent
            // operation are performed.
            pi_evt_sig_init(&end_task);

            // Initiate SPI slave send
            pi_spi_send_async(&spi_slave, spi_slave_rx_buf, CHUNK_SIZE_BITS, SPI_NO_OPTION, pi_evt_callback_irq_init(&send_task, &send_callback, &end_task));

            // Initiate SPI master receive
            //pi_spi_receive_async(&spi_master, (spi_master_rx_buf + (i*CHUNK_SIZE)), CHUNK_SIZE_BITS, SPI_NO_OPTION, pi_evt_callback_irq_init(&receive_task, &receive_callback, (void*)&end_task));

            // Blocking the execution until send and receive steps are done.
            pi_evt_wait(&end_task);
        }
        printf("slave -> master test done\n");

        // // Verify received data
        // for(int i = 0; i < BUFFER_SIZE; i++)
        // {
        //     if(spi_master_tx_buf[i] != spi_master_rx_buf[i])
        //     {
        //         printf("ERROR: index (%ld), expected (0x%lx), got M:(0x%lx), S:(0x%lx)\n",
        //             i, spi_master_tx_buf[i], spi_master_rx_buf[i], spi_slave_rx_buf[i]);
        //         retval = -1;
        //         break;
        //     }
        // }

        // // Testing data shift
        // if(retval)
        // {
        //     if(spi_master_tx_buf[1] == spi_slave_rx_buf[0])
        //     {
        //         printf("ERROR: First byte not read. All data are shifted by 1 byte\n");
        //         printf("INFO: Wrong dummy clock cycle configuration\n");
        //     }
        // }
        // else
        // {
        //     printf("INFO: Data received successfully\n");
        // }
    }
    printf("\n\t *** End of test ***\n\n");
    return retval;
}
