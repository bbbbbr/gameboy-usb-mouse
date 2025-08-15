#ifndef _USB_MOUSE_H
#define _USB_MOUSE_H

#include <stdint.h>
#include <stdbool.h>

#define MOUSE_BUTTON_LEFT          0x01u
#define MOUSE_BUTTON_RIGHT         0x02u
#define MOUSE_BUTTON_MIDDLE        0x04u  // Usually. Other fancy side buttons may be 0x08, 0x10, etc
#define MOUSE_BUTTON_MASK          (MOUSE_BUTTON_LEFT | MOUSE_BUTTON_RIGHT | MOUSE_BUTTON_MIDDLE)
#define X_DIR_POS                  0x10u
#define Y_DIR_POS                  0x20u

// The design with the AVR in between GB and CH559 means
// it's not possible to tell the difference between no data (0x00) and zero
//so force all bytes to be non-zero using various bits as indicators
#define FORCE_NONZERO_BIT_BUTTONS  0x40u
#define FORCE_NONZERO_BIT_MOVE     0x80u

#define X_MOVE_MAX                 127   // Max value without MSBit set
#define X_MOVE_MASK                0x7Fu
#define Y_MOVE_MAX                 127   // Max value without MSBit set
#define Y_MOVE_MASK                0x7Fu

#define SIO_IDX_INCREMENT_WRAP(var) if (var == SIO_BUF_SZ) var = 0u;

#define SIO_PACKET_SZ         3u
#define SIO_PACKET_BUF_COUNT  60u  // SZ x COUNT should not be > 255
#define SIO_BUF_SZ            (SIO_PACKET_SZ * SIO_PACKET_BUF_COUNT)


typedef struct mouse_data_t {
    uint8_t buttons_and_flags;
    uint8_t move_x;
    uint8_t move_y;
} mouse_data_t;

// extern mouse_data_t mouse;

extern uint8_t sio_rx_data[SIO_BUF_SZ];
extern uint8_t sio_write_head;
extern uint8_t sio_read_tail;
extern uint8_t sio_count;


bool mouse_is_data_ready(void);

void mouse_read_start(void);
void mouse_ISR_SIO(void);
void mouse_init(void);
void mouse_deinstall(void);

#endif // _USB_MOUSE_H
