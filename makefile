all: client stm32

clean:
	make -C client clean
	make -C stm32f091 clean

client_compile_commands:
	compiledb -o client/compile_commands.json make -BnC client

client:
	make -j -C client

stm32:
	make -j -C stm32f091
