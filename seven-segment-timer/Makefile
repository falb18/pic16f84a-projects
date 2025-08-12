PROJECT= seven-segment-timer

CC=sdcc
CCFLAGS=--use-non-free
FAMILY=pic14
PROC=16f84a

SRC=main.c

OBJS = $(SRC:.c=.o)
BIN_DIR = bin
OBJS := $(addprefix $(BIN_DIR)/, $(OBJS))
OUTPUT_FILES := $(addprefix $(BIN_DIR)/, *.cod *.lst *.hex *.asm *.o)

PK2CMD := pk2cmd-x86_64.AppImage

all: $(BIN_DIR)/$(PROJECT).hex

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)
	@echo "Created directory: $(BIN_DIR)"

$(BIN_DIR)/$(PROJECT).hex: $(OBJS) | $(BIN_DIR)
	@echo
	@echo $<: 
	$(CC) $(CCFLAGS) -m$(FAMILY) -p$(PROC) $^ -o $@

$(BIN_DIR)/%.o: %.c | $(BIN_DIR)
	@echo
	@echo $<:
	$(CC) -c $(CCFLAGS) -m$(FAMILY) -p$(PROC) $< -o $@

print-%: ; @echo $* = $($*)

.PHONY: clean flash

clean:
	rm $(OUTPUT_FILES)

# Tool options:
# -X use VPP first to entry in programming mode
# -M erased and programm all memory regions
# -R release MCLR pin after flashing
# -F flash file
flash:
	$(PK2CMD) -P PIC16F84A -X -M -R -F $(BIN_DIR)/$(PROJECT).hex