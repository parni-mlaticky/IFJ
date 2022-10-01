CC=gcc
CFLAGS=
LIBS=
TARGET=./ifj22

BUILD_DIR=./increment/

ifeq ($(release),1)
	CFLAGS += -o3
else
	CFLAGS += -g3
endif

OBJ = $(patsubst %.c,$(BUILD_DIR)%.o,$(wildcard *.c))

# Git doesn't store empty dirs so make sure BUILD_DIR is created
$(shell mkdir $(BUILD_DIR) 2> /dev/null || true)

.PHONY: clean run all

# Just so make all works as well
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) && echo "OK" >/dev/stderr

$(BUILD_DIR)%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(TARGET)
	./$<
clean:
	rm -f $(BUILD_DIR)/*.o
	rm $(TARGET)

