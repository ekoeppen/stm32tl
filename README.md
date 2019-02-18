# STM32 C++ Template Library

This library uses C++ templates to produce tight, compile time optimized and checked code for STM32F0/F1/F4 MCUs.

The library covers MCU initialization and interfacing with the MCU peripherals. On top of that, a number of
example drivers for external components can be found in the `drivers` directory.

Support code for a set of development boards is under the `boards` directory. The support code usually declares LEDs, buttons
and other components, and handles the board initialization.

Peripherals and drivers are implemented as C++ templates, which are instantiated via parameters describing the
underlying hardware. This allows the compiler to statically check and resolve dependencies.
