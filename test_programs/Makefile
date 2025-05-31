# Liste automatique de tous les fichiers .asm
ASM_FILES := $(wildcard *.asm)
BIN_FILES := $(ASM_FILES:.asm=.bin)

# Assembleur Z80
SJASMPLUS := sjasmplus.exe

# Target par défaut : construire tous les .bin
all: $(BIN_FILES)

# Règle générique pour convertir .asm en .bin
%.bin: %.asm
	$(SJASMPLUS) $< --raw=$@

# Nettoyage
clean:
	rm -f *.bin

.PHONY: all clean
