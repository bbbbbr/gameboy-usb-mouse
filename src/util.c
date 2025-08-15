#include <gbdk/platform.h>
#include <stdint.h>

void apa_exit(void) {
    set_interrupts(IE_REG & ~LCD_IFLAG);

    // Remove the two APA interrupt routines
    // Turn off LCD Interrupt
    // This disables and then re-enables interrupts so it must be outside the critical section.
    set_interrupts(IE_REG & ~LCD_IFLAG);

    // Remove the APA LCD interrupt routine (with interrupts turned off)
    CRITICAL {
        __asm \
            push BC
            push HL

            LD      BC, #.drawing_lcd
            LD      HL, #.int_0x48
            CALL    .remove_int

            pop HL
            pop BC
        __endasm;
    }
}