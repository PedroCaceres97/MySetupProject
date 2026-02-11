# ===============================
# User Build Configuration
# ===============================
# This file is included by Makefile
# You can freely edit it.
# Missing file will NOT cause errors.
# ===============================

# -------- Feature toggles --------
# Set to 1 to enable
ENABLE_SANITIZERS :=
ENABLE_WARNINGS_AS_ERRORS :=
ENABLE_LTO :=

# -------- Include directories --------
# Example:
# USER_INCLUDES += -Ithirdparty/imgui
USER_INCLUDES := -IC:\Dev\mystd

# -------- Library search paths --------
# Example:
# USER_LIB_PATHS += -Lthirdparty/lib
USER_LIB_PATHS :=

# -------- Libraries to link --------
# Example:
# USER_LIBS += -lm
# USER_LIBS += -lSDL2
USER_LIBS :=

# -------- Extra compiler flags --------
USER_CFLAGS :=

# -------- Extra linker flags --------
USER_LDFLAGS :=

# -------- Custom build hooks --------
# These are appended to default rules
PRE_BUILD :=
POST_BUILD :=

# -------- Source overrides --------
# Advanced usage:
# USER_SRC_FILES := extra/foo.c extra/bar.c
USER_SRC_FILES :=
