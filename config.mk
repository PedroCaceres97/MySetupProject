# =====================================
# MySetupProject config.mk template v0.4
# =====================================
# This file simplifies the use of custom build options.
# Read Makefile for more information
# =====================================

# -------- Feature toggles --------
# Set to 1 to enable
USER_ENABLE_SANITIZERS := 0
USER_ENABLE_WARNINGS_AS_ERRORS := 0
USER_ENABLE_LTO := 0

# -------- Include directories --------
# Example:
# USER_INCLUDES := -Ithirdparty/imgui
USER_INCLUDES := -IC:\dev\mystd

# -------- Library search paths --------
# Example:
# USER_LIB_PATHS := -Lthirdparty/lib
USER_LIB_PATHS :=

# -------- Libraries to link --------
# Example:
# USER_LIBS := -lm -lSDL2
USER_LIBS :=

# -------- Extra compiler flags --------
USER_CFLAGS :=

# -------- Extra linker flags --------
USER_LDFLAGS :=

# -------- Custom build hooks --------
# These are appended to default rules
USER_PRE_BUILD :=
USER_POST_BUILD :=
