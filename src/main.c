#define MY_LOG_COLOURED
#include <mystd\stdlib.c>
#include <mystd\stdio.c>
#include <mystd\argv-parser.c>

#ifdef MY_OS_WINDOWS
    #include <direct.h>
    #define chdir _chdir
#else
    #include <unistd.h>
#endif

#include <MySetupProject/templates.h>

MyArgvParserFlag project = {
    .long_name = "project",
    .listener = false,
    .value = {0},
    .short_name = 'p',
    .expect_value = false
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
    MyMakeDir("src");
    MyMakeDir("bin");
    MyMakeDir("build/debug");
    MyMakeDir("build/release");
    MyMakeDir(MySprintf("include/%s", project.value));
    MyMakeDir(".vscode");
}
void WriteMakefile() {
    MyFile* makefile = MyFileOpen("Makefile", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(makefile, makefileTemplate);
    MyFileClose(makefile);

    makefile = MyFileOpen("config.mk", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(makefile, makefileConfigTemplate);
    MyFileClose(makefile);

    makefile = MyFileOpen(".clangd", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(makefile, clangdTemplate);
    MyFileClose(makefile);

    makefile = MyFileOpen(".vscode/settings.json", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(makefile, settingsTemplate);
    MyFileClose(makefile);
}
void WriteMain() {
    MyFile* main = MyFileOpen("src/main.c", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(main, mainTemplate);
    MyFileClose(main);
}
void WriteGit() {
    const char* date = MY_TERNARY(year.listener, year.value, "<YEAR>");
    const char* auth = MY_TERNARY(author.listener, author.value, "<AUTHOR>");
    if (!year.listener) { MyLog(MY_WARNING, "Missing year writting '<YEAR>' as a default, to provide a year use --year=[value] or -y[value]"); }
    if (!author.listener) { MyLog(MY_WARNING, "Missing author writting '<AUTHOR>' as a default, to provide an author use --author=[value] or -a[value]"); }

    MyFile* git = MyFileOpen("LICENSE", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(git, "MIT License\n\n");
    MyFprintf(git, "Copyright (c) %s %s\n\n", date, auth);
    MyFilePrint(git, MITLicense);
    MyFileClose(git);

    git = MyFileOpen(".gitignore", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
    MyFilePrint(git, gitignoreTemplate);
    MyFileClose(git);
}

int main(int argc, char** argv) {
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
    MyArgvParser_Parse(&parser, (const char**)&argv[1], argc - 1);

    bool8 newProject = true;

    if (dirs.listener) {
        newProject = false;
        MyMakeDir("src");
        MyMakeDir("bin");
        MyMakeDir("build/debug");
        MyMakeDir("build/release");
        MyLog(MY_SUCCESS, "Basic directories were succesfully created");
    }

    if (MIT.listener) {
        newProject = false;
        const char* date = MY_TERNARY(year.listener, year.value, "<YEAR>");
        const char* auth = MY_TERNARY(author.listener, author.value, "<AUTHOR>");
        if (!year.listener) { MyLog(MY_WARNING, "Missing year writting '<YEAR>' as a default, to provide a year use --year=[value] or -y[value]"); }
        if (!author.listener) { MyLog(MY_WARNING, "Missing author writting '<AUTHOR>' as a default, to provide an author use --author=[value] or -a[value]"); }
        MyFile* git = MyFileOpen("LICENSE", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
        MyFilePrint(git, "MIT License\n\n");
        MyFprintf(git, "Copyright (c) %s %s\n\n", date, auth);
        MyFilePrint(git, MITLicense);
        MyFileClose(git);
        MyLog(MY_SUCCESS, "Latest MIT LICENSE template written");
    }

    if (makefile.listener) {
        newProject = false;
        MyFile* makefile = MyFileOpen("Makefile", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
        MyFilePrint(makefile, makefileTemplate);
        MyFileClose(makefile);
        MyLog(MY_SUCCESS, "Latest Makefile template written");
    }

    if (mkconfig.listener) {
        MyLog(MY_WARNING, "Writting latest config.mk template will erase any change.");
        MyPrintf("(Insert 'y' to confirm) > ");
        char confirm = getchar();
        if (confirm != 'y' && confirm != 'Y') {
            MyLog(MY_WARNING, "Latest config.mk aborted");
        } else {
            MyFile* makefile = MyFileOpen("config.mk", MY_FILE_FLAG_WRITE | MY_FILE_FLAG_NEW);
            MyFilePrint(makefile, makefileConfigTemplate);
            MyFileClose(makefile);
            MyLog(MY_SUCCESS, "Latest config.mk template written");
        }
    }

    if (newProject) {
        if (!project.listener) { MyLog(MY_FATAL, "Missing project name, to provide a project name use --project=[value] or -p[value]"); }
        MyMakeDir(project.value);
        chdir(project.value);
        MakeDirectories();
        WriteMakefile();
        WriteMain();
        WriteGit();
        if (githubPublic.listener || githubPrivate.listener) {
            system("git init --initial-branch=main");
            system(MySprintf("gh repo create %s %s --source=. --remote=origin", project.value, MY_TERNARY(githubPublic.listener, "--public", "--private")));
            system("git add .");
            system("git commit -m \"Initial Commit\"");
            system("git push -u origin main");
        }
    }
}