all: client

ifeq ($(wildcard client.mk),)
client.mk:
	qmake client.pro -o client.mk
endif

include client.mk

OBJECTS += $(patsubst %.c,%.o, $(wildcard ../shared/*.c))
client: $(OBJECTS)

../shared/%.o: ../shared/%.c
	$(CC) -c $(CFLAGS) -w $< -o $@
