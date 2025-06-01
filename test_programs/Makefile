ASM_FILES := $(wildcard *.asm)
BIN_FILES := $(ASM_FILES:.asm=.bin)

SJASMPLUS := sjasmplus.exe

OUTPUT_DIR ?= .

all: $(BIN_FILES)

%.bin: %.asm
	$(SJASMPLUS) $< --raw=${OUTPUT_DIR}/$@

clean:
	rm -f *.bin

.PHONY: all clean
