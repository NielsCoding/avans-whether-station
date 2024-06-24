CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OC = arm-none-eabi-objcopy
RM = rm -f

TARGET = main
HOST=$(strip $(shell uname -o))

OBJS += $(patsubst %.c,%-stm.o, $(wildcard ../shared/*.c))

SHARED_FLAGS += -g
SHARED_FLAGS += -DSTM32F091xC
SHARED_FLAGS += -Wall
SHARED_FLAGS += -Wextra
# SHARED_FLAGS += -Wno-register
SHARED_FLAGS += -Wa,--defsym,CALL_ARM_SYSTEM_INIT=1
SHARED_FLAGS += -I./lib/STM32-base-STM32Cube/HAL/STM32F0xx/inc
SHARED_FLAGS += -I./lib/STM32-base-STM32Cube/HAL/STM32F0xx/inc/Legacy
SHARED_FLAGS += -I./lib/STM32-base-STM32Cube/CMSIS/ARM/inc
SHARED_FLAGS += -I./lib/STM32-base-STM32Cube/CMSIS/STM32F0xx/inc
SHARED_FLAGS += -I./lib/STM32-base/startup
SHARED_FLAGS += -I./lib/FreeRTOS-Kernel/include
SHARED_FLAGS += -I./lib/FreeRTOS-Kernel/portable/GCC/ARM_CM0/
SHARED_FLAGS += -I.
ifeq ($(HOST),GNU/Linux)
SHARED_FLAGS += -I/usr/arm-none-eabi/include/
endif
# SHARED_FLAGS += -O1
SHARED_FLAGS += -ffunction-sections
SHARED_FLAGS += -fdata-sections
SHARED_FLAGS += -Wl,--gc-sections
SHARED_FLAGS += -mlittle-endian
SHARED_FLAGS += -mthumb
SHARED_FLAGS += -specs=nosys.specs
SHARED_FLAGS += -fno-exceptions
SHARED_FLAGS += -fno-unwind-tables
SHARED_FLAGS += -Wl,-L./lib/STM32-base/linker,-T./lib/STM32-base/linker/STM32F0xx/STM32F091xC.ld
SHARED_FLAGS += -mcpu=cortex-m0
SHARED_FLAGS += -march=armv6-m

CFLAGS += $(SHARED_FLAGS)
LFLAGS += $(SHARED_FLAGS)
AFLAGS += $(SHARED_FLAGS)

OBJS += $(patsubst %.c,%.o, $(wildcard *.c))
OBJS += ./lib/STM32-base/startup/STM32F0xx/STM32F091xC.o
OBJS += ./lib/STM32-base-STM32Cube/CMSIS/STM32F0xx/src/system_stm32f0xx.o
OBJS += lib/FreeRTOS-Kernel/croutine.o \
        lib/FreeRTOS-Kernel/event_groups.o \
        lib/FreeRTOS-Kernel/list.o \
        lib/FreeRTOS-Kernel/queue.o \
        lib/FreeRTOS-Kernel/stream_buffer.o \
        lib/FreeRTOS-Kernel/tasks.o \
        lib/FreeRTOS-Kernel/timers.o \
        lib/FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.o \
        lib/FreeRTOS-Kernel/portable/MemMang/heap_4.o
OBJS += lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_rcc.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_rcc_ex.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_i2c.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_i2c_ex.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_gpio.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_dma.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_cortex.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_pwr.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_pwr_ex.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_flash.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_flash_ex.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_tim.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_tim_ex.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_uart.o \
        lib/STM32-base-STM32Cube/HAL/STM32F0xx/src/stm32f0xx_hal_uart_ex.o

.PHONY: flash clean

$(TARGET).bin: $(TARGET).elf
	$(OC) -O binary $< $@

%.o: %.s
	$(CC) -c $(AFLAGS) $< -o $@

lib/%.o: lib/%.c
	$(CC) -c $(CFLAGS) -w $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%-stm.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET).elf: $(OBJS)
	$(LD) $(LFLAGS) $^ -o $@

flash: $(TARGET).bin
	st-flash --reset write $(TARGET).bin 0x08000000

compile_commands: clean
	compiledb make -n
	../scripts/compiledb-full-path-mingw.sh compile_commands.json

clean:
	$(RM) $(TARGET).bin $(TARGET).elf $(OBJS)

