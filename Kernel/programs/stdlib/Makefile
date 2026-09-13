FILES=./build/stdlib.o ./build/kernel.asm.o ./build/start.o
INCLUDES=-I./src
FLAGS= -g

all: ${FILES}
	i686-elf-ld -m elf_i386 -relocatable ${FILES} -o ./stdlib.elf
	
./build/start.o: ./src/start.asm
	nasm -f elf ./src/start.asm -o ./build/start.o

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf ./src/kernel.asm -o ./build/kernel.asm.o

./build/stdlib.o: 
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/stdlib.c -o ./build/stdlib.o

clean:
	rm -rf ${FILES}
	rm ./stdlib.elf