#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#include "usb_mouse.h"

uint8_t sio_rx_data[SIO_BUF_SZ];
uint8_t sio_write_head;
uint8_t sio_read_tail;
uint8_t sio_count;


// mouse_data_t mouse_data;



// Returns whether polling the mouse via serial interrupts is
// complete and data is ready for use
bool mouse_is_data_ready(void) {
    return (sio_count >= SIO_PACKET_SZ);
}


// Call this to start the process of reading the mouse via serial
// interrupt transfers
void mouse_read_start(void) {

    // Start a new transfer, the SIO interrupt will fire when it's done
    SB_REG = 0x00u;
    SC_REG = SIOF_XFER_START | SIOF_CLOCK_INT;
}


void mouse_ISR_SIO(void) {

    // Ignore bytes with value zero
    if (SB_REG != 0x00u) {

        // Only write if there is space    
        if (sio_count != SIO_BUF_SZ) {
            sio_rx_data[sio_write_head] = SB_REG;
            sio_write_head++;
            SIO_IDX_INCREMENT_WRAP(sio_write_head);
            sio_count++;
        }
    }
    SB_REG = 0x00u;
    SC_REG = SIOF_XFER_START | SIOF_CLOCK_INT;    
}



void mouse_init(void) {

    CRITICAL {
        uint8_t sio_write_head = 0u;
        uint8_t sio_read_tail  = 0u;
        uint8_t sio_count      = 0u;

        // Remove first to avoid accidentally double-adding it
        remove_SIO(mouse_ISR_SIO);
        add_SIO(mouse_ISR_SIO);
    }

    // Enable Serial interrupt
    set_interrupts(IE_REG | SIO_IFLAG);

    // Initiate a request to prime the interrupt read loop
    mouse_read_start();
}


// Called in de-install the mouse SIO interrupt
void mouse_deinstall(void) {

    CRITICAL {
        uint8_t sio_write_head = 0u;
        uint8_t sio_read_tail  = 0u;
        uint8_t sio_count      = 0u;

        remove_SIO(mouse_ISR_SIO);
    }
    set_interrupts(IE_REG & ~SIO_IFLAG);
}
