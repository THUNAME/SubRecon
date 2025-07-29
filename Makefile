CC = gcc
CFLAGS = -lm -lpthread -mcmodel=medium -fPIC
SRC = src/config.c src/hash.c src/construct.c src/budget.c src/parser.c
BIN_DIR = bin

DELIM_SRC = src/delimitation_main.c $(SRC)
AUGMENT_SRC = src/expansion_main.c $(SRC)

DELIM_BIN = $(BIN_DIR)/delimitation_main
AUGMENT_BIN = $(BIN_DIR)/expansion_main

# Ensure bin directory exists
$(shell mkdir -p $(BIN_DIR))

all: $(DELIM_BIN) $(AUGMENT_BIN)

$(DELIM_BIN): $(DELIM_SRC)
	$(CC) $(DELIM_SRC) -o $(DELIM_BIN) $(CFLAGS)

$(AUGMENT_BIN): $(AUGMENT_SRC)
	$(CC) $(AUGMENT_SRC) -o $(AUGMENT_BIN) $(CFLAGS)

clean:
	rm -f $(DELIM_BIN) $(AUGMENT_BIN)
