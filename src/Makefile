AS     = arm-none-eabi-as
CC     = arm-none-eabi-gcc
LIBGCC = `arm-none-eabi-gcc -print-libgcc-file-name`
CFLAGS = -Wall -O -fno-builtin -fomit-frame-pointer -finhibit-size-directive \
	-fno-ident

all: racket.bin

racket.bin: ball.o crt.o racket.o box.o score.o
	arm-none-eabi-ld -o racket.out -T gcc.ls \
	  crt.o ball.o racket.o box.o score.o  ${LIBGCC}
	arm-none-eabi-objcopy -O binary racket.out racket.bin

clean:
	rm -f *.o *.s *.out *.bin

