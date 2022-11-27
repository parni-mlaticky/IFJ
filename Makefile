CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -pedantic
LIBS :=
TARGET := ./ifj22


BUILD_DIR := ./increment/
RUN_FLAGS := test-scripts/test.php

ifeq ($(release),1)
	CFLAGS += -o3
else
	CFLAGS += -g3
endif

ifeq ($(strict),1)
	CFLAGS += -Werror
endif


OBJ = $(patsubst %.c,$(BUILD_DIR)%.o,$(wildcard *.c))

# Git doesn't store empty dirs so make sure BUILD_DIR is created
$(shell mkdir $(BUILD_DIR) 2> /dev/null || true)

.PHONY: clean run all test

# Just so make all works as well
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) && echo "OK" >/dev/stderr

$(BUILD_DIR)%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(TARGET)
	./$< $(RUN_FLAGS)

test: $(TARGET)
	$(TARGET) test-scripts/test.php >test.php.output
	@printf "`tput bold`Testing test.php`tput sgr0`"
	@echo ""
	@echo "test.php Output differences:"
	@echo ""
	-@diff --color -su test.php.output test-scripts/outputs/test.output || echo -e "\e[31mTEST FAILED\e[0m"
	@rm -f test.php.output
	@echo ""

	$(TARGET) test-scripts/escape_sequences_hex_octal.php | od -x >escape_sequences_hex_octal.php.output
	@printf "`tput bold`Testing escape sequences hex octal`tput sgr0`"
	@echo ""
	@echo "escape_sequences_hex_octal.php Output differences:"
	@echo ""
	-@diff --color -su escape_sequences_hex_octal.php.output test-scripts/outputs/escape_sequences_hex_octal.output || echo -e "\e[31mTEST FAILED\e[0m"
	@rm -f escape_sequences_hex_octal.php.output
	@echo ""


	$(TARGET) test-scripts/escape_sequences.php >escape_sequences.php.output
	@printf "`tput bold`Testing escape sequences`tput sgr0`"
	@echo ""
	@echo "escape_sequences.php Output differences:"
	@echo ""
	-@diff --color -su escape_sequences.php.output test-scripts/outputs/escape_sequences.output || echo -e "\e[31mTEST FAILED\e[0m"
	@rm -f escape_sequences.php.output
	@echo ""

	$(TARGET) test-scripts/escape_sequences_error.php >escape_sequences_error.php.output
	@printf "`tput bold`Testing escape sequences error`tput sgr0`"
	@echo ""
	@echo "escape_sequences_error.php Output differences:"
	@echo ""
	-@diff --color -su escape_sequences_error.php.output test-scripts/outputs/escape_sequences_error.output || echo -e "\e[31mTEST FAILED\e[0m"
	@rm -f escape_sequences_error.php.output




clean:
	rm -f $(BUILD_DIR)/*.o
	rm $(TARGET)

