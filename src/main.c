#include <mystd\stdlib.c>

#ifdef MY_OS_WINDOWS
    #include <direct.h>
    #define chdir _chdir
    #define getcwd _getcwd
#else
    #include <unistd.h>
#endif

#include <MySetupProject/flags.h>
#include <MySetupProject/templates.h>

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
    const char* date = MY_TERNARY(year.trigged, year.value, "<YEAR>");
    const char* auth = MY_TERNARY(author.trigged, author.value, "<AUTHOR>");
    if (!year.trigged) { MyLog(MY_WARNING, "Missing year writting '<YEAR>' as a default, to provide a year use --year=[value] or -y[value]"); }
    if (!author.trigged) { MyLog(MY_WARNING, "Missing author writting '<AUTHOR>' as a default, to provide an author use --author=[value] or -a[value]"); }
    
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
    MyPrintf("\n");

    MY_ASSERT(argc > 1,"No arguments were provided try: MySetupProject --project=[value] or MySetupProject --dirs");

    bool help = MyArgvParse(flags, sizeof(flags) / sizeof(MyArgvFlag*), &argv[1], argc - 1, UnkownFlagCallback);
    if (help) { return 0; }

    bool newProject = true;

    if (version.trigged) {
        newProject = false;
        MyPrintf("MySetupProject v0.5 (Compiled under %s mode)\n\n",
            #ifdef NDEBUG
                "release"
            #else
                "debug"
            #endif
        );
    }

    if (dirs.trigged) {
        newProject = false;
        MakeDirectories();
        MyLog(MY_SUCCESS, "Directories were succesfully created");
    }

    if (MIT.trigged) {
        newProject = false;
        WriteMIT();
        MyLog(MY_SUCCESS, "Latest MIT LICENSE template written");
    }

    if (makefile.trigged) {
        newProject = false;
        WriteMakefile();
        MyLog(MY_SUCCESS, "Latest Makefile template written");
    }

    if (mkconfig.trigged) {
        newProject = false;
        WriteSensible("config.mk", makefileConfigTemplate);
    }

    if (mktargets.trigged) {
        newProject = false;
        WriteSensible("targets.mk", makefileTargetsTemplate);
    }

    if (clangd.trigged) {
        newProject = false;
        WriteSensible(".clangd", clangdTemplate);
    }

    if (settings.trigged) {
        newProject = false;
        MyMakeDir(".vscode");
        WriteSensible(".vscode/settings.json", settingsTemplate);
    }

    if (gitignore.trigged) {
        newProject = false;
        WriteSensible(".gitignore", gitignoreTemplate);
    }

    if (newProject) {
        if (!project.trigged) { MyLog(MY_FATAL, "Missing project name, to provide a project name use --project=[value] or -p[value]"); }
        MyMakeDir(project.value);
        chdir(project.value);

        MakeDirectories();
        WriteMIT();
        WriteMktargets();
        WriteMkconfig();
        WriteMakefile();
        WriteGenerics();
        if (githubPublic.trigged || githubPrivate.trigged) {
            system("git init --initial-branch=main");
            system(MySprintf("gh repo create %s %s --source=. --remote=origin", project.value, MY_TERNARY(githubPublic.trigged, "--public", "--private")));
            system("git add .");
            system("git commit -m \"Initial Commit\"");
            system("git push -u origin main");
        }
        MyLog(MY_SUCCESS, "Project created");
    }
}