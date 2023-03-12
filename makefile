BUILD_DIR = ./build
SRC_DIR = ./src
INCLUDE_DIR = ./include

CC = gcc
CFLAGS = -Wall -Wextra -Werror -I$(INCLUDE_DIR)

LDFLAGS = -lm

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

all: run

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

compile: $(OBJ)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/main $(OBJ) -lm

run: compile
	echo "\nRunning program:"
	$(BUILD_DIR)/main

clean:
	rm -rf $(BUILD_DIR)/*