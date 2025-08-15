# GBDK/C Mouse Example for Inside Gadgets USB adapter

Example for using a Inside Gadgets Keyboard adapter modified
to work with a mouse. The adapter is connected to the link port.

https://shop.insidegadgets.com/product/gameboy-usb-keyboard-adapter/

Requires the modified firmware at:
xxxxx

Once the mouse interrupt is initialized and the data is started,
it reads the link port connected to the AVR part continually to
check for data. When data is found (non-zero) it's stored into
a ring buffer and read out from the main loop.

Each mouse transfer has 3 bytes: buttons & flags, x and y.

