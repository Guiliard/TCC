CC        := gcc
CFLAGS    := -Wall -Wextra -O3 -Iinclude -I$(GUROBI_HOME)/include
# LDFLAGS   := -L$(GUROBI_HOME)/lib -lgurobi130 -lm # Gurobi Optimizer version 13.0.1
LDFLAGS   := -L$(GUROBI_HOME)/lib -lgurobi120 -lm # Gurobi Optimizer version 12.0.1

SRC_DIR   := src
BUILD_DIR := build/objects
TARGET    := build/rscpGRB

SRCS      := $(shell find $(SRC_DIR) -name '*.c')
OBJS      := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SRCS:.c=.o))

all: create_dirs $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

create_dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(dir $(TARGET))
	@mkdir -p build/log

clean:
	rm -rf build/

run: all
	./$(TARGET)

.PHONY: all create_dirs clean run