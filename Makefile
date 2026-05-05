CC        := gcc
CFLAGS    := -Wall -Wextra -Werror -fopenmp -Iinclude/ -I../concorde -I../concorde/INCLUDE -I../qsopt
LDFLAGS   := -fopenmp
SRC_DIR   := src
BUILD_DIR := build/objects
TARGET    := build/executable

SRCS      := $(shell find $(SRC_DIR) -name '*.c')
OBJS      := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SRCS:.c=.o))

all: create_dirs $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ concorde/concorde.a qsopt/lib/qsopt.a -lpthread -lm $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

create_dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(dir $(TARGET))

clean:
	rm -rf build/

run: all
	./$(TARGET)

.PHONY: all create_dirs clean run