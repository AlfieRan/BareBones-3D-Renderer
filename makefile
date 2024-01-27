BUILD_DIR = ./build
SRC_DIR = ./src
INCLUDE_DIR = ./include

# These flags are for my specific setup, you should remove/change them as needed
ADDITIONAL_FLAGS = -I/usr/local/include -L/usr/local/lib -lSDL3

CC = gcc
CFLAGS = -Wall -Wextra -I$(INCLUDE_DIR) ${ADDITIONAL_FLAGS} -O3

LDFLAGS = -lm

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

all: clean run

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

compile: $(OBJ)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/main $(OBJ) -lm

run: compile
	echo "\nRunning program:"
	$(BUILD_DIR)/main

clean:
	rm -rf $(BUILD_DIR)/*