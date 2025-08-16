# GBDK/C Mouse Example for Inside Gadgets USB adapter
Example for using a Inside Gadgets Keyboard adapter modified
to work with a mouse. The adapter is connected to the link port.

https://github.com/user-attachments/assets/03dc71b7-e846-4c97-9b48-a62c6c032300

Adapter:
https://shop.insidegadgets.com/product/gameboy-usb-keyboard-adapter/

Requires the modified firmware at:
https://github.com/bbbbbr/CH559sdccUSBHost/

Once the mouse interrupt is initialized and the data is started,
it reads the link port connected to the AVR part continually to
check for data. When data is found (non-zero) it's stored into
a ring buffer and read out from the main loop.

Each mouse transfer has 3 bytes: buttons & flags, x and y.

The CH559 firmware may be somewhat specific to some models of logitech mice
right now (M510 works). The solution may be switching connected mice into "boot"
mode. Otherwise the firmware would need be to changed on a per-device basis 
depending on how the mouse packs it's buttons and movements into the HID reports.
