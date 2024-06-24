# avans-whether-station

## important details

- the client subfolder uses qmake to generate `client/makefile`, thus only
  `client/client.pro` should be tracked under version control, not
  `client/makefile`
- the stm32 makefile uses git submodules to pull necessary files for
  compilation. make sure to initialize and sync the git submodules, or use the
  `--recursive` flag when cloning
- the stm32 firmware expects the esp8266 module to have the official espressif
  firmware, not the ai-thinker firmware that the module comes pre-flashed with

## support

this project is currently being developed and tested on the host platforms
listed below. toolchain installation scripts can be found in the
[scripts](scripts) subdirectory.

||Arch Linux<br>(x86_64)|Windows 10<br>(x86_64)|MacOS Monterey<br>(arm/apple m1)|
|-|-|-|-|
|STM32 makefile compilation|yes|yes|?|
|STM32 makefile upload|yes|yes|?|
|STM32 debugging (vscode)|yes|yes|?|
|STM32 editor autocomplete|yes|yes|?|
|QT client compilation (qmake)|yes|?|?|
|QT client running|yes|?|?|
|QT client debugging (vscode)|yes|?|?|
|QT client editor autocomplete|yes|?|?|
