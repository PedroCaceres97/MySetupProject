#define MY_LOG_COLOURED
#include <mystd\stdio.h>
#include <mystd\stdlib.c>
#include <mystd\stdio.c>

#ifdef MY_OS_WINDOWS
    #include <direct.h>
    #define chdir _chdir
#else
    #include <unistd.h>
#endif

const char* TargetName = NULL;
bool author_set = false;
bool year_set = false;
char author[128] = {0};
char year[128] = {0};

const char* ClangdTemplate = 
"CompileFlags:\n"
"\tAdd: [-xc, -std=gnu23]"
"";

const char* MainTemplate = 
"#include <stdio.h>\n"
"\n"
"int main() {\n"
"\tprintf(\"Hello World\\n\");\n"
"\treturn 0;\n"
"}\n"
"";

const char* MakefileConfigTemplate = 
"# ===============================\n"
"# User Build Configuration\n"
"# ===============================\n"
"# This file is included by Makefile\n"
"# You can freely edit it.\n"
"# Missing file will NOT cause errors.\n"
"# ===============================\n"
"\n"
"# -------- Feature toggles --------\n"
"# Set to 1 to enable\n"
"ENABLE_SANITIZERS :=\n"
"ENABLE_WARNINGS_AS_ERRORS :=\n"
"ENABLE_LTO :=\n"
"\n"
"# -------- Include directories --------\n"
"# Example:\n"
"# USER_INCLUDES := -Ithirdparty/imgui\n"
"USER_INCLUDES :=\n"
"\n"
"# -------- Library search paths --------\n"
"# Example:\n"
"# USER_LIB_PATHS := -Lthirdparty/lib\n"
"USER_LIB_PATHS :=\n"
"\n"
"# -------- Libraries to link --------\n"
"# Example:\n"
"# USER_LIBS := -lm -lSDL2\n"
"USER_LIBS :=\n"
"\n"
"# -------- Extra compiler flags --------\n"
"USER_CFLAGS :=\n"
"\n"
"# -------- Extra linker flags --------\n"
"USER_LDFLAGS :=\n"
"\n"
"# -------- Custom build hooks --------\n"
"# These are appended to default rules\n"
"PRE_BUILD :=\n"
"POST_BUILD :=\n"
"\n"
"# -------- Source overrides --------\n"
"# Advanced usage:\n"
"# USER_SRC_FILES := $(wildcard $(SRC_FOLDER)/$(SUBDIR)/*.c)\n"
"USER_SRC_FILES :=\n"
"";

const char* MakefileTemplate =
"# ===============================\n"
"# Makefile template v0.3\n"
"# ===============================\n"
"# EDIT WITH PRECAUTION.\n"
"# ===============================\n"
"\n"
"# -------- Build options --------\n"
"MODE ?= Debug\n"
"CVERSION ?= 23\n"
"\n"
"# -------- Toolchain --------\n"
"CC := gcc\n"
"AR := ar\n"
"AS := nasm\n"
"\n"
"ifeq ($(OS),Windows_NT)\n"
"    MKDIR = if not exist \"$(1)\" mkdir \"$(1)\"\n"
"else\n"
"    MKDIR = mkdir -p \"$(1)\"\n"
"endif\n"
"\n"
"# -------- Base flags --------\n"
"BASE_CFLAGS := -Wall -Wextra -MMD -MP\n"
"DEBUG_CFLAGS := -g -O0\n"
"RELEASE_CFLAGS := -O3\n"
"\n"
"# -------- Project layout --------\n"
"SRC_FOLDER := src\n"
"BIN_FOLDER := bin\n"
"BUILD_FOLDER := build\n"
"\n"
"DEBUG_FOLDER := debug\n"
"RELEASE_FOLDER := release\n"
"\n"
"# -------- Mode selection --------\n"
"ifeq ($(MODE),Debug)\n"
"\tCFLAGS := $(DEBUG_CFLAGS)\n"
"\tBUILD_PATH := $(BUILD_FOLDER)/$(DEBUG_FOLDER)\n"
"else\n"
"\tCFLAGS := $(RELEASE_CFLAGS)\n"
"\tBUILD_PATH := $(BUILD_FOLDER)/$(RELEASE_FOLDER)\n"
"endif\n"
"\n"
"# -------- Language standard --------\n"
"ifneq ($(CVERSION),Default)\n"
"\tCFLAGS += -std=gnu$(CVERSION)\n"
"endif\n"
"\n"
"CFLAGS += $(BASE_CFLAGS)\n"
"\n"
"# -------- Defaults (safe empty) --------\n"
"USER_INCLUDES :=\n"
"USER_LIB_PATHS :=\n"
"USER_LIBS :=\n"
"USER_CFLAGS :=\n"
"USER_LDFLAGS :=\n"
"PRE_BUILD :=\n"
"POST_BUILD :=\n"
"\n"
"# -------- User overrides --------\n"
"-include config.mk\n"
"\n"
"# -------- Feature toggles --------\n"
"ifeq ($(ENABLE_SANITIZERS),1)\n"
"\tCFLAGS += -fsanitize=address,undefined\n"
"\tLDFLAGS += -fsanitize=address,undefined\n"
"endif\n"
"\n"
"ifeq ($(ENABLE_WARNINGS_AS_ERRORS),1)\n"
"\tCFLAGS += -Werror\n"
"endif\n"
"\n"
"ifeq ($(ENABLE_LTO),1)\n"
"\tCFLAGS += -flto\n"
"\tLDFLAGS += -flto\n"
"endif\n"
"\n"
"# -------- Final merged flags --------\n"
"INCLUDES := -Iinclude $(USER_INCLUDES)\n"
"CFLAGS   += $(USER_CFLAGS)\n"
"LDFLAGS  += $(USER_LDFLAGS)\n"
"\n"
"# -------- Source files --------\n"
"SRC_FILES := $(wildcard $(SRC_FOLDER)/*.c)\n"
"SRC_FILES += $(USER_SRC_FILES)\n"
"\n"
"OBJ_FILES := $(patsubst $(SRC_FOLDER)/%.c,$(BUILD_PATH)/%.o,$(SRC_FILES))\n"
"DEPS      := $(OBJ_FILES:.o=.d)\n"
"\n"
"TARGET_NAME := $(notdir $(CURDIR))\n"
"ifeq ($(MODE),Debug)\n"
"\tTARGET := $(BIN_FOLDER)/$(TARGET_NAME)-debug\n"
"else\n"
"\tTARGET := $(BIN_FOLDER)/$(TARGET_NAME)\n"
"endif\n"
"\n"
".PHONY: all clean\n"
"\n"
"all: $(TARGET)\n"
"\n"
"$(TARGET): $(OBJ_FILES)\n"
"\t$(PRE_BUILD)\n"
"\t@$(call MKDIR,$(dir $@))\n"
"\t$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(USER_LIB_PATHS) $(USER_LIBS) $(LDFLAGS)\n"
"\t$(POST_BUILD)\n"
"\n"
"$(BUILD_PATH)/%.o: $(SRC_FOLDER)/%.c\n"
"\t@$(call MKDIR,$(dir $@))\n"
"\t$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@\n"
"\n"
"-include $(DEPS)\n"
"";

const char* SettingsTemplate = 
"{\n"
"\t\"C_Cpp.intelliSenseEngine\": \"disabled\",\n"
"\t\"clangd.arguments\": [\n"
"\t\t\"--query-driver=**\",\n"
"\t\t\"--header-insertion=never\",\n"
"\t\t\"--background-index\",\n"
"\t\t\"--clang-tidy\",\n"
"\t\t\"--completion-style=detailed\"\n"
"\t]\n"
"}\n"
"";

const char* MITLicense = 
"Permission is hereby granted, free of charge, to any person obtaining a copy\n"
"of this software and associated documentation files (the \"Software\"), to deal\n"
"in the Software without restriction, including without limitation the rights\n"
"to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
"copies of the Software, and to permit persons to whom the Software is\n"
"furnished to do so, subject to the following conditions:\n"
"\n"
"The above copyright notice and this permission notice shall be included in all\n"
"copies or substantial portions of the Software.\n"
"\n"
"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
"SOFTWARE.\n"
"";

const char* gitignore =
"# Prerequisites\n"
"*.d\n"
"\n"
"# Object files\n"
"*.o\n"
"*.ko\n"
"*.obj\n"
"*.elf\n"
"\n"
"# Linker output\n"
"*.ilk\n"
"*.map\n"
"*.exp\n"
"\n"
"# Precompiled Headers\n"
"*.gch\n"
"*.pch\n"
"\n"
"# Libraries\n"
"*.lib\n"
"*.a\n"
"*.la\n"
"*.lo\n"
"\n"
"# Shared objects (inc. Windows DLLs)\n"
"*.dll\n"
"*.so\n"
"*.so.*\n"
"*.dylib\n"
"\n"
"# Executables\n"
"*.exe\n"
"*.out\n"
"*.app\n"
"*.i*86\n"
"*.x86_64\n"
"*.hex\n"
"\n"
"# Debug files\n"
"*.dSYM/\n"
"*.su\n"
"*.idb\n"
"*.pdb\n"
"\n"
"# Kernel Module Compile Results\n"
"*.mod*\n"
"*.cmd\n"
".tmp_versions/\n"
"modules.order\n"
"Module.symvers\n"
"Mkfile.old\n"
"dkms.conf\n"
"\n"
"# debug information files\n"
"*.dwo\n"
"\n"
"# Project\n"
".clangd\n"
".vscode/\n"
"bin/\n"
"build/\n"
"\n";

void MakeDirectories() {
    MyMakeDir("src");
    MyMakeDir("bin");
    MyMakeDir("build/debug");
    MyMakeDir("build/release");
    MyMakeDir(MySprintf("include/%s", TargetName));
    MyMakeDir(".vscode");
}

void WriteMakefile() {
    MyFile* makefile = MyFileOpen("Makefile", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(makefile, MakefileTemplate);
    MyFileClose(makefile);

    makefile = MyFileOpen("config.mk", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(makefile, MakefileConfigTemplate);
    MyFileClose(makefile);

    makefile = MyFileOpen(".clangd", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(makefile, ClangdTemplate);
    MyFileClose(makefile);

    makefile = MyFileOpen(".vscode/settings.json", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(makefile, SettingsTemplate);
    MyFileClose(makefile);
}

void WriteMain() {
    MyFile* main = MyFileOpen("src/main.c", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(main, MainTemplate);
    MyFileClose(main);
}

void WriteGit() {
    MyFile* git = MyFileOpen("LICENSE", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(git, "MIT License\n\n");
    MyFprintf(git, "Copyright (c) %s %s\n\n", MY_TERNARY(year_set, year, "<YEAR>"), MY_TERNARY(author_set, author, "<AUTHOR>"));
    MyFilePrint(git, MITLicense);
    MyFileClose(git);

    git = MyFileOpen(".gitignore", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(git, gitignore);
    MyFileClose(git);
}

int main(int argc, char** argv) {
    MY_ASSERT(argc > 1, "At least the project name must be passed as a parameter 'MyProject [name]' or 'MyProject -dirs'");
    
    if (strcmp(argv[1], "-dirs") == 0) {
        MakeDirectories();
        MyLog(MY_SUCCESS, "Directories were succesfully created");
        return 0;
    }
    if (strcmp(argv[1], "-makefile") == 0) {
        MyFile* makefile = MyFileOpen("Makefile", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
        MyFilePrint(makefile, MakefileTemplate);
        MyFileClose(makefile);
        MyLog(MY_SUCCESS, "Latest Makefile template written");
        return 0;
    }

    TargetName = argv[1];

    bool github_public = false;
    bool github_private = false;
    for (int i = 2; i < argc; i++) {
        if (argv[i][0] != '-') {
            MyLog(MY_LOG, MySprintf("Unknown argument: %s", argv[i]));
            continue;
        }

        if (argv[i][1] == 'a') {
            strncpy(author, argv[++i], sizeof(author));
            author_set = true;
            continue;
        } else if (argv[i][1] == 'd') {
            strncpy(year, argv[++i], sizeof(year));
            year_set = true;
            continue;
        }

        if (strcmp("-github", argv[i]) == 0) {
            github_public = true;
        } else if (strcmp("-github-private", argv[i]) == 0) {
            github_private = true;
        }
    }

    MyMakeDir(TargetName);
    chdir(TargetName);
    MakeDirectories();
    WriteMakefile();
    WriteMain();
    WriteGit();
    if (github_public || github_private) {
        system("git init --initial-branch=main");
        system(MySprintf("gh repo create %s %s --source=. --remote=origin", TargetName, MY_TERNARY(github_public, "--public", "--private")));
        system("git add .");
        system("git commit -m \"Initial Commit\"");
        system("git push -u origin main");
    } 
}