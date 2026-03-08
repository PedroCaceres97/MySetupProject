# ===============================
# Makefile template v0.3
# ===============================
# EDIT WITH PRECAUTION.
# ===============================

# -------- Build options --------
MODE ?= Debug
CVERSION ?= 23

# -------- Toolchain --------
CC := gcc
AR := ar
AS := nasm

ifeq ($(OS),Windows_NT)
    MKDIR = if not exist "$(1)" mkdir "$(1)"
else
    MKDIR = mkdir -p "$(1)"
endif

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
	BUILD_PATH := $(BUILD_FOLDER)/$(DEBUG_FOLDER)
else
	CFLAGS := $(RELEASE_CFLAGS)
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

TARGET_NAME := $(notdir $(CURDIR))
ifeq ($(MODE),Debug)
	TARGET := $(BIN_FOLDER)/$(TARGET_NAME)-debug
else
	TARGET := $(BIN_FOLDER)/$(TARGET_NAME)
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(PRE_BUILD)
	@$(call MKDIR,$(dir $@))
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(USER_LIB_PATHS) $(USER_LIBS) $(LDFLAGS)
	$(POST_BUILD)

$(BUILD_PATH)/%.o: $(SRC_FOLDER)/%.c
	@$(call MKDIR,$(dir $@))
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

-include $(DEPS)
