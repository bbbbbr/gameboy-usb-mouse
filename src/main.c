#include <gbdk/platform.h>
#include <stdint.h>
#include <stdio.h>

#include <gb/drawing.h>

#include "common.h"
#include "input.h"
#include "util.h"

#include "usb_mouse.h"

uint8_t sample_num = 0;

enum {
    POLL_MOUSE,
    POLL_GAMEPAD
};


#define SPRITE_MOUSE_CURSOR 0u

const unsigned char mouse_cursors[] = {
  // Arrow 1
  0xFE, 0xFE, 0xFC, 0x84, 0xF8, 0x98, 0xF8, 0xA8,
  0xFC, 0xB4, 0xCE, 0xCA, 0x87, 0x85, 0x03, 0x03,
  // Arrow 2
  0x80, 0x80, 0xC0, 0xC0, 0xE0, 0xA0, 0xF0, 0x90,
  0xF8, 0x88, 0xF0, 0xB0, 0xD8, 0xD8, 0x08, 0x08,
  // Arrow 3
  0x80, 0x80, 0xC0, 0xC0, 0xE0, 0xA0, 0xF0, 0x90,
  0xF8, 0x88, 0xFC, 0x84, 0xF8, 0x98, 0xE0, 0xE0,
  // Hand
  0x10, 0x10, 0x38, 0x28, 0x38, 0x28, 0x7E, 0x6E,
  0xFE, 0xA2, 0xFE, 0x82, 0x7E, 0x42, 0x3E, 0x3E
};


static void main_init(void) {

    HIDE_BKG;
    HIDE_SPRITES;

    set_sprite_data(0u, 2u, mouse_cursors);
    set_sprite_tile(SPRITE_MOUSE_CURSOR, 1u);
    move_sprite(0, 160u / 2, 144u / 2);

    DISPLAY_ON;
    SPRITES_8x8;

    SHOW_BKG;
    SHOW_SPRITES;

	UPDATE_KEYS();
}


#define MOUSE_LAST_UNSET 0xFFu
void use_mouse_data(void) {

    static uint8_t last_x = MOUSE_LAST_UNSET;
    static uint8_t last_y = MOUSE_LAST_UNSET;

    // Loop through all data in the RX buffer
    while (mouse_is_data_ready()) {

        // Copy mouse data out of RX buffer
        mouse_data_t mouse = *((mouse_data_t *)&sio_rx_data[sio_read_tail]);

        // Increment read tail and free the data in the RX buffer
        sio_read_tail += SIO_PACKET_SZ;
        SIO_IDX_INCREMENT_WRAP(sio_read_tail);
        CRITICAL {
            sio_count -= SIO_PACKET_SZ;
        }

        // printf("%hx,%hx,%hx\n", (uint8_t)mouse.buttons_and_flags, (uint8_t)mouse.move_x, (uint8_t)mouse.move_y);

        // If all bytes are 255 then the adapter is probably not connected
        if (mouse.move_x == mouse.move_y == mouse.buttons_and_flags == 0xffu)
            return;

        // Make sure the data is aligned right (only x and y bytes have bit .7 set)
        if ((((mouse.move_x & mouse.move_y) & FORCE_NONZERO_BIT_MOVE) != 0x00u) && 
             ((mouse.buttons_and_flags & FORCE_NONZERO_BIT_BUTTONS) != 0x00u)) {

            uint8_t mouse_buttons = (mouse.buttons_and_flags & MOUSE_BUTTON_MASK);

            int8_t mouse_x_move = (mouse.move_x & X_MOVE_MASK);
            if (!(mouse.buttons_and_flags & X_DIR_POS)) mouse_x_move *= -1;

            int8_t mouse_y_move = (mouse.move_y & Y_MOVE_MASK);
            if (!(mouse.buttons_and_flags & Y_DIR_POS)) mouse_y_move *= -1;

            scroll_sprite(SPRITE_MOUSE_CURSOR, mouse_x_move, mouse_y_move);

            if (mouse_buttons & MOUSE_BUTTON_LEFT) {
                OAM_item_t * itm = &shadow_OAM[SPRITE_MOUSE_CURSOR];
                uint8_t x = itm->x - DEVICE_SPRITE_PX_OFFSET_X + SCX_REG;
                uint8_t y = itm->y - DEVICE_SPRITE_PX_OFFSET_Y + SCY_REG;

                if (last_x == MOUSE_LAST_UNSET) {
                    plot_point(x, y);
                } else {
                    line(last_x, last_y, x, y);
                }
                // Save coordinates for next time
                last_x = x;
                last_y = y;
            } else {
                last_x = last_y = MOUSE_LAST_UNSET;
            }

            if (mouse_buttons & MOUSE_BUTTON_RIGHT) {
                scroll_bkg(-mouse_x_move, -mouse_y_move);
            }

            if (mouse_buttons & MOUSE_BUTTON_MIDDLE) {
                box(0u,0u, DEVICE_SCREEN_PX_WIDTH - 1, DEVICE_SCREEN_PX_HEIGHT -1, M_FILL);
            }
        }
    }
}



void mouse_poll_loop(void) {

    mode(M_DRAWING);
    plot_point(0,0);

    // Start reading the mouse
    mouse_init();

    UPDATE_KEYS();
    while (1) {
        // Check for exit
        UPDATE_KEYS();
        if (KEY_TICKED(J_ANY)) break;

        // Check if data ready for use
        use_mouse_data();

        vsync();
    }

    mouse_deinstall();

    // This is a little broken still    
    apa_exit();
    mode(M_TEXT_OUT);
    SCX_REG = 0u;
    SCY_REG = 0u;
}


void main(void){

    UPDATE_KEYS();

    printf(
      "Mouse \n"
      " ST: Live Draw\n");

    main_init();

    while (1) {

        UPDATE_KEYS();
        if (KEY_TICKED(J_START)) mouse_poll_loop();

        vsync();
    }
}

