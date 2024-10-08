# set these environment variables in your .basrc or windows environment
# to automagically copy the FUJIAPPLE.po file to the TNFS server

# .bashrc
# TNFS_SERVER="192.168.2.21" or if in windows "\\server"
# TNFS_SHARE="tnfs" - the name of the share
# export TNFS_SERVER
# export TNFS_SHARE
#
# windows
# SET TNFS_SERVER=tnfs
# SET TNFS_SHARE=tnfs
#
# TNFS_DIR - the directory within the share


# COLEM_EXEC Coleco vision emulator executible with full path 
# NABEM_PATH NABU emulator executible path
# NABEM_EXEC NABU emulator executible name
# APPEM_EXEC Apple emualtor executible with full path

PROJECT_DEVICE := adam

TARGET_EXEC ?= sabotage

ifeq ('$(PROJECT_DEVICE)', 'adam')
	EXTENSION := .ddp
else
	EXTENSION := 
endif

ifeq ('$(OS)','')
	WINE_REQUIRED := wine
else
	WINE_REQUIRED := 
endif

TNFS_SERVER ?= $(shell echo $$TNFS_SERVER)
TNFS_SHARE  ?= $(shell echo $$TNFS_SHARE)
TNFS_DIR    ?= ADAM
OS          ?= $(shell echo $$OS)

COLEM_EXEC  ?= $(shell echo $$COLEM_EXEC)
COLEM_WINE  :=
NABEM_PATH  ?= $(shell echo $$NABEM_PATH)
NABEM_EXEC  ?= $(shell echo $$NABEM_EXEC)
NABEM_WINE  := WINE_REQUIRED
APPEM_EXEC  ?= $(shell echo $$APPEM_EXEC)
APPEM_WINE  := WINE_REQURED

NABU_CACHE  ?= $(shell echo $$NABU_CACHE)
NABU_WINE   := WINE_REQURED


BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

CC=zcc
AS=zcc

ADDL_DIR1 := ../eoslib/src
ADDL_DIR2 := ../smartkeyslib/src
ADDL_DIR3 := src

ADDL_LIB1 := ../eoslib/eos.lib
ADDL_LIB2 := ../smartkeyslib/smartkeys.lib

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.asm)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CFLAGS=+coleco -subtype=adam -O3 -DBUILD_ADAM  -I$(ADDL_DIR1) -I$(ADDL_DIR2) -I$(ADDL_DIR3)
LDFLAGS=+coleco -subtype=adam  -m -s  -o$(TARGET_EXEC) -create-app -l$(ADDL_LIB1) -l$(ADDL_LIB2) 
ASFLAGS=+coleco -subtype=adam

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS)
	$(RM) *BOOTSTRAP.bin

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# asm source
$(BUILD_DIR)/%.asm.o: %.asm
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c -o$@  $< 


.PHONY: clean

clean:
	$(RM) -r $(TARGET_EXEC)* $(BUILD_DIR)

go:
ifeq ('$(PROJECT_DEVICE)','adam')
	$(COLEM_WINE) "$(COLEM_EXEC)" -ntsc -adam $(TARGET_EXEC).ddp
endif
ifeq ('$(PROJECT_DEVICE)','apple')
	$(WINE_REQUIRED) "$(APPEM_EXEC)" -d1 $(TARGET_EXEC).po
endif
ifeq ('$(PROJECT_DEVICE)','nabu')
	cp ./$(TARGET_EXEC) "$(NABU_CACHE)/000001.nabu"
	cd "$(NABEM_PATH)"; \
	$(WINE_REQUIRED) "$(NABEM_PATH)/$(NABEM_EXEC)" nabupc -window -kbd nabu_hle -hcca null_modem -bitb socket.127.0.0.1:5816
endif

tnfs: $(TARGET_EXEC).ddp 
ifneq ('$(TNFS_SERVER)','')

ifneq ('$(OS)','')
	cp $(TARGET_EXEC)$(EXTENSION). //$(TNFS_SERVER)/$(TNFS_SHARE)/$(TNFS_DIR)
else
	cp $(TARGET_EXEC)$(EXTENSION) /run/user/1000/gvfs/smb-share:server=$(TNFS_SERVER),share=$(TNFS_SHARE)/$(TNFS_DIR)
endif
else
	$(info ***********************************************)
	$(info *** No TNFS server environment variable set ***)
	$(info ***********************************************)
endif


-include $(DEPS)

MKDIR_P ?= mkdir -p



