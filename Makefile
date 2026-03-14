# =====================================
# MySetupProject Makefile template v0.4
#
# Update logs v0.4:
#   - Update logs were introduced
#   - Toolchain variables now are setted with ?=
#   - RELEASE_CFLAGS uses -O2 instead of -O3
#	- Standard build mode changed to release
#	- Now is possible to compile multiple targets:
#		Targets are generated dynamically, to implement a new target
#		open targets.mk and add the target name into zTARGETS
#		then you must generate [target]_DIR wich contains the root directory
#		of the source files (admits subdirs) and [target]_TYPE := (executable / static)
#	- TODO: Add shared libraries compilation
# =====================================
# EDIT WITH PRECAUTION.
# =====================================

# -------- Crossplatform --------

ifeq ($(OS),Windows_NT)
    MKDIR = mkdir "$(1)" 2>nul || exit 0
    RMDIR = if exist "$(1)" rmdir /S /Q "$(1)"
else
    MKDIR = mkdir -p "$(1)"
    RMDIR = rm -rf "$(1)"
endif

# Recursive wildcard
rwildcard = $(filter $(subst *,%,$2),$(wildcard $1/$2)) \
            $(foreach d,$(wildcard $1/*),$(call rwildcard,$d,$2))

# -------- Project layout --------
SRC_FOLDER := src
OBJ_FOLDER := obj
BIN_FOLDER := bin
LIB_FOLDER := lib

DEBUG_FOLDER := debug
RELEASE_FOLDER := release

# -------- Build tools --------

MODE ?= release
CVERSION ?= 23

CC := gcc
AR := ar
AS := nasm

CFLAGS ?= -Iinclude -Wall -Wextra -MMD -MP
ifneq ($(CVERSION),Default)
	CFLAGS += -std=gnu$(CVERSION)
endif

LDFLAGS ?= 

# -------- Release or Debug specifics --------

DEBUG_CFLAGS := -g -O0 -DDEBUG	
RELEASE_CFLAGS := -O2 -DNDEBUG

ifeq ($(MODE),release)
	CFLAGS += $(RELEASE_CFLAGS)
	LIB_PATH 	:= $(LIB_FOLDER)/$(RELEASE_FOLDER)
	BIN_PATH 	:= $(BIN_FOLDER)/$(RELEASE_FOLDER)
	BUILD_PATH 	:= $(OBJ_FOLDER)/$(RELEASE_FOLDER)
else
	CFLAGS += $(DEBUG_CFLAGS)
	LIB_PATH 	:= $(LIB_FOLDER)/$(DEBUG_FOLDER)
	BIN_PATH 	:= $(BIN_FOLDER)/$(DEBUG_FOLDER)
	BUILD_PATH 	:= $(OBJ_FOLDER)/$(DEBUG_FOLDER)
endif


# -------- User Configuration --------

-include config.mk

USER_INCLUDES ?=
USER_LIB_PATHS ?=
USER_LIBS ?=
USER_CFLAGS ?=
USER_LDFLAGS ?=
USER_PRE_BUILD ?=
USER_POST_BUILD ?=

USER_ENABLE_SANITIZERS ?= 0
USER_ENABLE_WARNINGS_AS_ERRORS ?= 0
USER_ENABLE_LTO ?= 0

ifeq ($(USER_ENABLE_SANITIZERS),1)
	CFLAGS += -fsanitize=address,undefined
	LDFLAGS += -fsanitize=address,undefined
endif

ifeq ($(USER_ENABLE_WARNINGS_AS_ERRORS),1)
	CFLAGS += -Werror
endif

ifeq ($(USER_ENABLE_LTO),1)
	CFLAGS += -flto
	LDFLAGS += -flto
endif

CFLAGS  += $(USER_INCLUDES) $(USER_CFLAGS)
LDFLAGS += $(USER_LIB_PATHS) $(USER_LDFLAGS)

# -------- Phony targets --------

include targets.mk

.PHONY: all clean

all: $(TARGETS)

clean:
	@$(RMDIR) $(OBJ_FOLDER)
	@$(RMDIR) $(BIN_FOLDER)
	@$(RMDIR) $(LIB_FOLDER)

# -------- Target generator --------

define DEFINE_TARGET

$(1)_DIR ?= src
$(1)_TYPE ?= executable

$(1)_SRC_FILES := $$(call rwildcard,$$($(1)_DIR),*.c)
$(1)_REL_SRC := $$(patsubst $$($(1)_DIR)/%,%,$$($(1)_SRC_FILES))
$(1)_OBJ_FILES := $$(patsubst %.c,$$(BUILD_PATH)/$(1)/%.o,$$($(1)_REL_SRC))
$(1)_DEPS_FILES := $$($(1)_OBJ_FILES:.o=.d)

ifeq ($$($(1)_TYPE),executable)

$(1): $$(BIN_PATH)/$(1)

$$(BIN_PATH)/$(1): $$($(1)_OBJ_FILES)
	@$(call MKDIR,$$(dir $$@))
	$$(USER_PRE_BUILD)
	$$(CC) $$(CFLAGS) $$^ -o $$@ $$(LDFLAGS) $$(USER_LIBS)
	$$(USER_POST_BUILD)

endif

ifeq ($$($(1)_TYPE),static)

$(1): $$(LIB_FOLDER)/lib$(1).a

$$(LIB_FOLDER)/lib$(1).a: $$($(1)_OBJ_FILES)
	@$(call MKDIR,$$(dir $$@))
	$$(USER_PRE_BUILD)
	$$(AR) rcs $$@ $$^
	$$(USER_POST_BUILD)

endif

$$(BUILD_PATH)/$(1)/%.o: $$($(1)_DIR)/%.c
	@$$(call MKDIR,$$(dir $$@))
	$$(CC) $$(CFLAGS) -c $$< -o $$@

-include $$($(1)_DEPS_FILES)

endef

$(foreach target,$(TARGETS),$(eval $(call DEFINE_TARGET,$(target))))
