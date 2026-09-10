BUILD_DIR := build
SRCS := generator.c qdbmp.c test.c
OBJS := $(addprefix $(BUILD_DIR)/, $(patsubst %.c,%.o,$(SRCS)))
DEPS := $(addprefix $(BUILD_DIR)/, $(patsubst %.c,%.d,$(SRCS)))
EXE = $(BUILD_DIR)/gen
LD = gcc
CFLAGS = -Wall -Wextra
COMPILE_OPTS = -MMD -MP -c

TEST_DIR := $(BUILD_DIR)/test
TEST_SRCS = qdbmp.c generator.c test.c
TEST_OBJS = $(addprefix $(TEST_DIR)/, $(patsubst %.c,%.o,$(TEST_SRCS)))
TEST_DEPS = $(addprefix $(TEST_DIR)/, $(patsubst %.c,%.d,$(TEST_SRCS)))
TEST_EXE = $(TEST_DIR)/test

.PHONY: debug
debug: CFLAGS += -ggdb
debug: COMPILE_OPTS += -ggdb
debug: $(EXE)

$(BUILD_DIR)/.:
	mkdir -p $(BUILD_DIR)

$(EXE): $(OBJS)
	$(LD) $(CFLAGS) $^ -o $@
	ln -frsT $@ $(notdir $@)

.SECONDEXPANSION:
$(BUILD_DIR)/%.o: %.c | $$(@D)/.
	$(LD) $(COMPILE_OPTS) $< -o $@

.PHONY: testing
testing: CFLAGS += -ggdb
testing: COMPILE_OPTS += -ggdb
testing: $(TEST_EXE)

$(TEST_DIR)/.:
	mkdir -p $(TEST_DIR)/

$(TEST_EXE): $(TEST_OBJS)
	$(LD) $(CFLAGS) $^ -o $@
	ln -frsT $@ $(notdir $@)

.SECONDEXPANSION:
$(TEST_DIR)/%.o: %.c | $$(@D)/.
	$(LD) $(COMPILE_OPTS) $< -o $@

.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR)/ $(notdir $(EXE)) $(notdir $(TEST_EXE))

-include $(DEPS)
-include $(TEST_DEPS)
