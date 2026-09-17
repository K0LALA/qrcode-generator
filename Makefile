BUILD_DIR := build
SRCS := generator.c qdbmp.c main.c
OBJS := $(addprefix $(BUILD_DIR)/, $(patsubst %.c,%.o,$(SRCS)))
DEPS := $(addprefix $(BUILD_DIR)/, $(patsubst %.c,%.d,$(SRCS)))
EXE = $(BUILD_DIR)/gen
CC = gcc
CFLAGS = -std=c23 -pedantic -Wall -Wextra -MMD -MP -c
DO_UNIT_TESTS = 1

.PHONY: debug
ifeq ($(DO_UNIT_TESTS), 1)
debug: CFLAGS += -DUNIT_TESTS
endif
debug: CC += -ggdb
debug: $(EXE)

$(BUILD_DIR)/.:
	mkdir -p $(BUILD_DIR)

$(EXE): $(OBJS)
	$(CC) $^ -o $@
	ln -frsT $@ $(notdir $@)

.SECONDEXPANSION:
$(BUILD_DIR)/%.o: %.c | $$(@D)/.
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR)/ $(notdir $(EXE))

-include $(DEPS)
