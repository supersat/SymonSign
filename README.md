# SymonSign

This project directly drives Symon LED signs from an Arduino Duemilanove.
It accepts 2-bit color bitmaps over the virtual serial port. Due to memory
limmitations, only four colors (red, green, yellow, and off) are supported.
However, the serial interface is fast enough to support host-side PWM.

The Duemilanove is the *only* supported hardware. The Uno's USB interface
is not fast enough to support 2 Mbps serial streams. The SparkFun RedBoard
does not break out the FTDI's flow control lines.
