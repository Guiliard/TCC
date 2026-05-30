CC        := gcc
CFLAGS 	  := -Wall -Wextra -Werror -fopenmp
LDFLAGS   := -fopenmp
SRC_DIR   := src
BUILD_DIR := build/objects
TARGET    := build/executable

LP_SOLVER ?= CPLEX

ifeq ($(LP_SOLVER),CPLEX)
LP_LIB = cplex/libcplex.a
endif

ifeq ($(LP_SOLVER),QSOPT)
LP_LIB = qsopt/qsopt.a
endif

SRCS      := $(shell find $(SRC_DIR) -name '*.c')
OBJS      := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SRCS:.c=.o))

all: create_dirs $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ concorde/concorde.a $(LP_LIB) -lpthread -lm $(LDFLAGS) -o $@

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