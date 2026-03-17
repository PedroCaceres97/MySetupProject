#define MY_LOG_COLOURED
#define MY_DEBUG_DISABLE
#include <mystd\stdlib.c>

#ifdef MY_OS_WINDOWS
    #include <direct.h>
    #define chdir _chdir
    #define getcwd _getcwd
#else
    #include <unistd.h>
#endif

#include <MySetupProject/templates.h>

MyArgvFlag version = {
    .value = {0},
    .longName = "version",
    .shortName = 'v',
    .expectValue = false,
    .listener = false
};

MyArgvFlag project = {
    .value = {0},
    .longName = "project",
    .shortName = 'p',
    .expectValue = true,
    .listener = false
};

MyArgvFlag dirs = {
    .value = {0},
    .longName = "dirs",
    .expectValue = false,
    .shortName = 'd',
    .listener = false
};

MyArgvFlag author = {
    .value = {0},
    .longName = "author",
    .shortName = 'a',
    .expectValue = true,
    .listener = false
};

MyArgvFlag year = {
    .value = {0},
    .longName = "year",
    .shortName = 'y',
    .expectValue = true,
    .listener = false
};

MyArgvFlag MIT = {
    .value = {0},
    .longName = "mit",
    .shortName = 0,
    .expectValue = false,
    .listener = false
};

MyArgvFlag makefile = {
    .value = {0},
    .longName = "makefile",
    .shortName = 0,
    .expectValue = false,
    .listener = false
};

MyArgvFlag mkconfig = {
    .value = {0},
    .longName = "mkconfig",
    .shortName = 0,
    .expectValue = false,
    .listener = false
};

MyArgvFlag mktargets = {
    .value = {0},
    .longName = "mktargets",
    .shortName = 0,
    .expectValue = false,
    .listener = false
};

MyArgvFlag clangd = {
    .value = {0},
    .longName = "clangd",
    .shortName = 0,
    .expectValue = false,
    .listener = false
};

MyArgvFlag settings = {
    .value = {0},
    .longName = "settings",
    .shortName = 0,
    .expectValue = false,
    .listener = false
};

MyArgvFlag gitignore = {
    .value = {0},
    .longName = "gitignore",
    .shortName = 0,
    .expectValue = false,
    .listener = false
};

MyArgvFlag githubPublic = {
    .value = {0},
    .longName = "github",
    .shortName = 'g',
    .expectValue = false,
    .listener = false
};

MyArgvFlag githubPrivate = {
    .value = {0},
    .longName = "github-private",
    .shortName = 0,
    .expectValue = false,
    .listener = false
};

MyArgvFlag* flags[] = {
    &version, 
    &project, 
    &dirs, 
    &author, 
    &year, 
    &MIT, 
    &makefile, 
    &mkconfig, 
    &mktargets, 
    &clangd, 
    &settings, 
    &gitignore, 
    &githubPublic, 
    &githubPrivate
};

void UnkownFlagCallback(const char* arg) {
    MyLog(MY_WARNING, "Unkown flag -> %s", arg);
}

void WriteSensible(const char* filename, const char* template) {
    MyLog(MY_WARNING, MySprintf("Writting latest %s template will erase any change.", filename));
    MyPrintf("(Insert 'y' to confirm) > ");
    char confirm = getchar();
    if (confirm != 'y' && confirm != 'Y') {
        MyLog(MY_WARNING, MySprintf("Latest %s aborted", filename));
        return;
    }

    MyFile* file = MyFileOpen(filename, MY_FILE_WRITE | MY_FILE_NEW);
    MyFilePrint(file, template);
    MyFileClose(file);
    MyLog(MY_SUCCESS, "Latest config.mk template written");
}

void MakeDirectories() {
    char current[512] = {0};
    getcwd(current, sizeof(current));
    MY_ASSERT(strlen(current) > 0, "Failed getcwd");
    MyLog(MY_DEBUG, current);
    char* last = MyLastPathDivisor(current);
    MY_ASSERT(last, "Invalid value returned by getcwd() -> %s", current);

    MyMakeDir("src");
    MyMakeDir("bin/debug");
    MyMakeDir("bin/release");
    MyMakeDir("obj/debug");
    MyMakeDir("obj/release");
    MyMakeDir("lib/debug");
    MyMakeDir("lib/release");
    MyMakeDir(".vscode");
    MyMakeDir(MySprintf("include/%s", last));
}
void WriteMakefile() {
    MyFile* makefile = MyFileOpen("Makefile", MY_FILE_WRITE | MY_FILE_NEW);
    MyFilePrint(makefile, makefileTemplate);
    MyFileClose(makefile);
}
void WriteMkconfig() {
    MyFile* mkconfig = MyFileOpen("config.mk", MY_FILE_WRITE | MY_FILE_NEW);
    MyFilePrint(mkconfig, makefileConfigTemplate);
    MyFileClose(mkconfig);
}
void WriteMktargets() {
    MyFile* mktargets = MyFileOpen("targets.mk", MY_FILE_WRITE | MY_FILE_NEW);
    MyFilePrint(mktargets, makefileTargetsTemplate);
    MyFileClose(mktargets);
}
void WriteMIT() {
    const char* date = MY_TERNARY(year.listener, year.value, "<YEAR>");
    const char* auth = MY_TERNARY(author.listener, author.value, "<AUTHOR>");
    if (!year.listener) { MyLog(MY_WARNING, "Missing year writting '<YEAR>' as a default, to provide a year use --year=[value] or -y[value]"); }
    if (!author.listener) { MyLog(MY_WARNING, "Missing author writting '<AUTHOR>' as a default, to provide an author use --author=[value] or -a[value]"); }
    
    MyFile* MITfile = MyFileOpen("LICENSE", MY_FILE_WRITE | MY_FILE_NEW);
    MyFilePrint(MITfile, "MIT License\n\n");
    MyFprintf(MITfile, "Copyright (c) %s %s\n\n", date, auth);
    MyFilePrint(MITfile, MITLicense);
    MyFileClose(MITfile);
}
void WriteGenerics() {
    if (!MyFileExists("src/main.c")) {
        MyFile* main = MyFileOpen("src/main.c", MY_FILE_WRITE | MY_FILE_NEW);
        MyFilePrint(main, mainTemplate);
        MyFileClose(main);
    }

    if (!MyFileExists(".gitignore")) {
        MyFile* gitignore = MyFileOpen(".gitignore", MY_FILE_WRITE | MY_FILE_NEW);
        MyFilePrint(gitignore, gitignoreTemplate);
        MyFileClose(gitignore);
    }

    if (!MyFileExists(".clangd")) {
        MyFile* clangd = MyFileOpen(".clangd", MY_FILE_WRITE | MY_FILE_NEW);
        MyFilePrint(clangd, clangdTemplate);
        MyFileClose(clangd);
    }

    if (!MyFileExists(".vscode/settings.json")) {
        MyFile* settings = MyFileOpen(".vscode/settings.json", MY_FILE_WRITE | MY_FILE_NEW);
        MyFilePrint(settings, settingsTemplate);
        MyFileClose(settings);
    }
}

int main(int argc, const char** argv) {
    if (argc == 1) {
        MyLog(MY_FATAL, "No arguments were provided try: MySetupProject --project=[value] or MySetupProject --dirs");
    }

    MyArgvParse(flags, sizeof(flags) / sizeof(MyArgvFlag*), &argv[1], argc - 1, UnkownFlagCallback);

    bool newProject = true;

    if (version.listener) {
        newProject = false;
        MyPrintf("MySetupProject v0.5 (Compiled under %s mode)\n\n",
            #ifdef NDEBUG
                "release"
            #else
                "debug"
            #endif
        );
    }

    if (dirs.listener) {
        newProject = false;
        MakeDirectories();
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
        WriteSensible("config.mk", makefileConfigTemplate);
    }

    if (mktargets.listener) {
        newProject = false;
        WriteSensible("targets.mk", makefileTargetsTemplate);
    }

    if (clangd.listener) {
        newProject = false;
        WriteSensible(".clangd", clangdTemplate);
    }

    if (settings.listener) {
        newProject = false;
        MyMakeDir(".vscode");
        WriteSensible(".vscode/settings.json", settingsTemplate);
    }

    if (gitignore.listener) {
        newProject = false;
        WriteSensible(".gitignore", gitignoreTemplate);
    }

    if (newProject) {
        if (!project.listener) { MyLog(MY_FATAL, "Missing project name, to provide a project name use --project=[value] or -p[value]"); }
        MyMakeDir(project.value);
        chdir(project.value);

        MakeDirectories();
        WriteMIT();
        WriteMktargets();
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
        MyLog(MY_SUCCESS, "Project created");
    }
}