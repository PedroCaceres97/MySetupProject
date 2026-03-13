#include "mystd/stdio.h"
#include "mystd/stdlib.h"
#define MY_LOG_COLOURED
#include <mystd\stdlib.c>
#include <mystd\stdio.c>
#include <mystd\argv-parser.c>

#ifdef MY_OS_WINDOWS
    #include <direct.h>
    #define chdir _chdir
    #define getcwd _getcwd
#else
    #include <unistd.h>
#endif

#include <MySetupProject/templates.h>

MyArgvParserFlag project = {
    .long_name = "project",
    .listener = false,
    .value = {0},
    .short_name = 'p',
    .expect_value = true
};

MyArgvParserFlag dirs = {
    .long_name = "dirs",
    .listener = false,
    .value = {0},
    .short_name = 0,
    .expect_value = false
};

MyArgvParserFlag MIT = {
    .long_name = "mit",
    .listener = false,
    .value = {0},
    .short_name = 0,
    .expect_value = false
};

MyArgvParserFlag makefile = {
    .long_name = "makefile",
    .listener = false,
    .value = {0},
    .short_name = 0,
    .expect_value = false
};

MyArgvParserFlag mkconfig = {
    .long_name = "mkconfig",
    .listener = false,
    .value = {0},
    .short_name = 0,
    .expect_value = false
};

MyArgvParserFlag author = {
    .long_name = "author",
    .listener = false,
    .value = {0},
    .short_name = 'a',
    .expect_value = true
};

MyArgvParserFlag year = {
    .long_name = "year",
    .listener = false,
    .value = {0},
    .short_name = 'y',
    .expect_value = true
};

MyArgvParserFlag githubPublic = {
    .long_name = "github",
    .listener = false,
    .value = {0},
    .short_name = 'g',
    .expect_value = false
};

MyArgvParserFlag githubPrivate = {
    .long_name = "github-private",
    .listener = false,
    .value = {0},
    .short_name = 0,
    .expect_value = false
};

void MakeDirectories() {
    char current[MY_MAX_PATH] = {0};
    getcwd(current, sizeof(current));
    MyNormalizePath(current);
    char* last = strrchr(current, '/');
    if (!last) { last = strrchr(current, '\\'); }
    MY_ASSERT(last, MySprintf("Invalid value returned by getchw() -> %s", current));

    MyMakeDir("src");
    MyMakeDir("bin");
    MyMakeDir("build/debug");
    MyMakeDir("build/release");
    MyMakeDir(".vscode");
    MyMakeDir(MySprintf("include/%s", current));
}
void WriteMakefile() {
    MyFile* makefile = MyFileOpen("Makefile", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(makefile, makefileTemplate);
    MyFileClose(makefile);
}
void WriteMkconfig() {
    MyFile* mkconfig = MyFileOpen("config.mk", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(mkconfig, makefileConfigTemplate);
    MyFileClose(mkconfig);
}
void WriteMIT() {
    const char* date = MY_TERNARY(year.listener, year.value, "<YEAR>");
    const char* auth = MY_TERNARY(author.listener, author.value, "<AUTHOR>");
    if (!year.listener) { MyLog(MY_WARNING, "Missing year writting '<YEAR>' as a default, to provide a year use --year=[value] or -y[value]"); }
    if (!author.listener) { MyLog(MY_WARNING, "Missing author writting '<AUTHOR>' as a default, to provide an author use --author=[value] or -a[value]"); }
    
    MyFile* MITfile = MyFileOpen("LICENSE", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(MITfile, "MIT License\n\n");
    MyFprintf(MITfile, "Copyright (c) %s %s\n\n", date, auth);
    MyFilePrint(MITfile, MITLicense);
    MyFileClose(MITfile);
}
void WriteGenerics() {
    if (!MyFileExists("src/main.c")) {
        MyFile* main = MyFileOpen("src/main.c", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
        MyFilePrint(main, mainTemplate);
        MyFileClose(main);
    }

    if (!MyFileExists(".gitignore")) {
        MyFile* gitignore = MyFileOpen(".gitignore", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
        MyFilePrint(gitignore, gitignoreTemplate);
        MyFileClose(gitignore);
    }

    if (!MyFileExists(".clangd")) {
        MyFile* clangd = MyFileOpen(".clangd", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
        MyFilePrint(clangd, clangdTemplate);
        MyFileClose(clangd);
    }

    if (!MyFileExists(".vscode/settings.json")) {
        MyFile* settings = MyFileOpen(".vscode/settings.json", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
        MyFilePrint(settings, settingsTemplate);
        MyFileClose(settings);
    }
}

int main(int argc, const char** argv) {
    if (argc == 1) {
        MyLog(MY_FATAL, "No arguments were provided try: MySetupProject --project=[value] or MySetupProject --dirs");
    }

    MyArgvParser parser = {0};
    MyArgvParser_Create(&parser);
    MyArgvParser_Register(&parser, &project);
    MyArgvParser_Register(&parser, &dirs);
    MyArgvParser_Register(&parser, &MIT);
    MyArgvParser_Register(&parser, &makefile);
    MyArgvParser_Register(&parser, &mkconfig);
    MyArgvParser_Register(&parser, &author);
    MyArgvParser_Register(&parser, &year);
    MyArgvParser_Register(&parser, &githubPublic);
    MyArgvParser_Register(&parser, &githubPrivate);
    MyArgvParser_Parse(&parser, argv, argc);

    bool8 newProject = true;

    if (dirs.listener) {
        newProject = false;
        MakeDirectories();
        WriteGenerics();
        MyLog(MY_SUCCESS, "Directories were succesfully created");
    }

    if (MIT.listener) {
        newProject = false;
        WriteMIT();
        MyLog(MY_SUCCESS, "Latest MIT LICENSE template written");
    }

    if (makefile.listener) {
        newProject = false;
        WriteMakefile();
        MyLog(MY_SUCCESS, "Latest Makefile template written");
    }

    if (mkconfig.listener) {
        newProject = false;
        MyLog(MY_WARNING, "Writting latest config.mk template will erase any change.");
        MyPrintf("(Insert 'y' to confirm) > ");
        char confirm = getchar();
        if (confirm != 'y' && confirm != 'Y') {
            MyLog(MY_WARNING, "Latest config.mk aborted");
        } else {
            WriteMkconfig();
            MyLog(MY_SUCCESS, "Latest config.mk template written");
        }
    }

    if (newProject) {
        if (!project.listener) { MyLog(MY_FATAL, "Missing project name, to provide a project name use --project=[value] or -p[value]"); }
        MyMakeDir(project.value);
        chdir(project.value);

        MakeDirectories();
        WriteMIT();
        WriteMkconfig();
        WriteMakefile();
        WriteGenerics();
        if (githubPublic.listener || githubPrivate.listener) {
            system("git init --initial-branch=main");
            system(MySprintf("gh repo create %s %s --source=. --remote=origin", project.value, MY_TERNARY(githubPublic.listener, "--public", "--private")));
            system("git add .");
            system("git commit -m \"Initial Commit\"");
            system("git push -u origin main");
        }
    }
}