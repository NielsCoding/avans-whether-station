# stm32 firmware subdirectory

- uses make
- make sure to initialize the git submodules
- all warnings from source files in the lib/ subfolder are hidden
- copy ../shared/wifi.def.h to ../shared/wifi.h and edit the network
  credentials
- the initialization code is broken in some way which means that a soft reset
  is required for the uart dma to work, either (a) press the reset button on
  the development board after plugging in, or (b) run `st-flash reset` after
  plugging in.

