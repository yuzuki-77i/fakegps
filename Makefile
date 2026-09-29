CC = gcc
CFLAGS = -Wall -Wextra -pedantic

SRC_DIR = repository
BINS_DIR = bin
SRCS = $(wildcard $(SRC_DIR)/fakegps*.c)
BINS = $(patsubst $(SRC_DIR)/%.c,$(BINS_DIR)/%,$(SRCS))

all: $(BINS)

$(BINS_DIR)/%: $(SRC_DIR)/%.c 
	mkdir -p $(BINS_DIR)
	$(CC) $(CFLAGS) $< -o $@

debug:
	@echo "SRCS = $(SRCS)"
	@echo "BINS = $(BINS)"

clean: 
	rm -rf $(BINS_DIR)

.PHONY: all clean debug

