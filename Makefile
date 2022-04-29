TARGET_EXEC ?= sabotage.ddp

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

ADDL_DIR1 := ../eoslib/src
ADDL_DIR2 := ../smartkeyslib/src
ADDL_DIR3 := ./src

ADDL_LIB1 := ../eoslib/eos.lib
ADDL_LIB2 := ../smartkeyslib/smartkeys.lib

CC=zcc
AS=zcc

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.asm)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CFLAGS=+coleco -subtype=adam -DBUILD_ADAM -I$(ADDL_DIR1) -I$(ADDL_DIR2) -I$(ADDL_DIR3)
LDFLAGS=+coleco -subtype=adam -o$(TARGET_EXEC) -create-app -lndos -l$(ADDL_LIB1) -l$(ADDL_LIB2)
ASFLAGS=+coleco -subtype=adam

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS)
	rm sabotage_BOOTSTRAP.bin


# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# asm source
$(BUILD_DIR)/%.asm.o: %.asm
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r sabotage* $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
