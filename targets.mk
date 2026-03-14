# =====================================
# MySetupProject targets.mk template v0.4
# =====================================
# This file defines the targets to be compiled.
# Read Makefile for more information
# =====================================

TARGET := $(notdir $(CURDIR))

TARGETS := $(TARGET)

$(TARGET)_DIR  := src
$(TARGET)_TYPE := executable
