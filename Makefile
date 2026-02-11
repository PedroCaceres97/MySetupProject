MODE ?= Debug
CVERSION ?= Default

# -------- Toolchain --------
CC ?= gcc
AR ?= ar
AS ?= nasm

# -------- Base flags --------
BASE_CFLAGS := -Wall -Wextra -MMD -MP
DEBUG_CFLAGS := -g -O0
RELEASE_CFLAGS := -O3

# -------- Project layout --------
SRC_FOLDER := src
BIN_FOLDER := bin
BUILD_FOLDER := build

DEBUG_FOLDER := debug
RELEASE_FOLDER := release

# -------- Mode selection --------
ifeq ($(MODE),Debug)
	CFLAGS := $(DEBUG_CFLAGS)
	BIN_PATH   := $(BIN_FOLDER)/$(DEBUG_FOLDER)
	BUILD_PATH := $(BUILD_FOLDER)/$(DEBUG_FOLDER)
else
	CFLAGS := $(RELEASE_CFLAGS)
	BIN_PATH   := $(BIN_FOLDER)/$(RELEASE_FOLDER)
	BUILD_PATH := $(BUILD_FOLDER)/$(RELEASE_FOLDER)
endif

# -------- Language standard --------
ifneq ($(CVERSION),Default)
	CFLAGS += -std=gnu$(CVERSION)
endif

CFLAGS += $(BASE_CFLAGS)

# -------- Defaults (safe empty) --------
USER_INCLUDES :=
USER_LIB_PATHS :=
USER_LIBS :=
USER_CFLAGS :=
USER_LDFLAGS :=
PRE_BUILD :=
POST_BUILD :=

# -------- User overrides --------
-include config.mk

# -------- Feature toggles --------
ifeq ($(ENABLE_SANITIZERS),1)
	CFLAGS += -fsanitize=address,undefined
	LDFLAGS += -fsanitize=address,undefined
endif

ifeq ($(ENABLE_WARNINGS_AS_ERRORS),1)
	CFLAGS += -Werror
endif

ifeq ($(ENABLE_LTO),1)
	CFLAGS += -flto
	LDFLAGS += -flto
endif

# -------- Final merged flags --------
INCLUDES := -Iinclude $(USER_INCLUDES)
CFLAGS   += $(USER_CFLAGS)
LDFLAGS  += $(USER_LDFLAGS)

# -------- Source files --------
SRC_FILES := $(wildcard $(SRC_FOLDER)/*.c)
SRC_FILES += $(USER_SRC_FILES)

OBJ_FILES := $(patsubst $(SRC_FOLDER)/%.c,$(BUILD_PATH)/%.o,$(SRC_FILES))
DEPS      := $(OBJ_FILES:.o=.d)

DIRS := $(BIN_PATH) $(BUILD_PATH)

TARGET_NAME := $(notdir $(CURDIR))
TARGET      := $(BIN_PATH)/$(TARGET_NAME)

.PHONY: all clean

all: $(TARGET)

# -------- Directories --------
$(BIN_FOLDER):
	@mkdir $(BIN_FOLDER)

$(BUILD_FOLDER):
	@mkdir $(BUILD_FOLDER)

$(BIN_PATH): | $(BIN_FOLDER)
	@mkdir $(BIN_PATH)

$(BUILD_PATH): | $(BUILD_FOLDER)
	@mkdir $(BUILD_PATH)

$(TARGET): $(OBJ_FILES) | $(BIN_PATH)
	$(PRE_BUILD)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(USER_LIB_PATHS) $(USER_LIBS) $(LDFLAGS)
	$(POST_BUILD)

$(BUILD_PATH)/%.o: $(SRC_FOLDER)/%.c | $(BUILD_PATH)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

-include $(DEPS)