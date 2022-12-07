CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -pedantic
LIBS :=
TARGET := ./ifj22

BUILD_DIR := ./
RUN_FLAGS :=
RUN_FILE_PATH := test-scripts/test.php

# Adds optimizations or debug info
ifeq ($(release),1)
	CFLAGS += -o3
else
	CFLAGS += -g3
endif

# Makes the compilation crash on warnings
ifeq ($(strict),1)
	CFLAGS += -Werror
endif

# Generating a list of object files that will need to be complied
OBJ = $(patsubst %.c,$(BUILD_DIR)%.o,$(wildcard *.c))


.PHONY: clean run all int intv

# Pople like the "all"
all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) && echo "OK" >/dev/stderr

# Building all the object files
$(BUILD_DIR)%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Build and run
run: $(TARGET)
	cat $(RUN_FILE_PATH) | ./$< $(RUN_FLAGS)

# Run and interpret
int: $(TARGET)
	cat $(RUN_FILE_PATH) | ./$< $(RUN_FLAGS) > prog.ifj && \
	./ic22int prog.ifj

# Run and interpret in debug mode
intv: $(TARGET)
	cat $(RUN_FILE_PATH) | ./$< $(RUN_FLAGS) > prog.ifj && \
	./ic22int -v prog.ifj

# As good as new
clean:
	rm -f $(BUILD_DIR)/*.o
	rm $(TARGET)

