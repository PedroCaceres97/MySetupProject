#include <mystd/stdlib.h>

MyArgvFlag version = {
    .value = {0},
    .description = "Shows current binary version",
    .longName = "version",
    .shortName = 'v',
    .expectValue = false,
    .trigged = false
};

MyArgvFlag project = {
    .value = {0},
    .description = "Sets the project name used to create root folder and github repositorie",
    .longName = "project",
    .shortName = 'p',
    .expectValue = true,
    .trigged = false
};

MyArgvFlag dirs = {
    .value = {0},
    .description = "Create base folders such as src/, bin/, obj/, ...",
    .longName = "dirs",
    .expectValue = false,
    .shortName = 'd',
    .trigged = false
};

MyArgvFlag author = {
    .value = {0},
    .description = "Sets the author name used in MIT license",
    .longName = "author",
    .shortName = 'a',
    .expectValue = true,
    .trigged = false
};

MyArgvFlag year = {
    .value = {0},
    .description = "Sets the year used in MIT license",
    .longName = "year",
    .shortName = 'y',
    .expectValue = true,
    .trigged = false
};

MyArgvFlag MIT = {
    .value = {0},
    .description = "Writes a copy of MIT license, optional --year and --author flags maybe used",
    .longName = "mit",
    .shortName = 0,
    .expectValue = false,
    .trigged = false
};

MyArgvFlag makefile = {
    .value = {0},
    .description = "Rewrites the Makefile template, doesnt ask for confirmation",
    .longName = "makefile",
    .shortName = 0,
    .expectValue = false,
    .trigged = false
};

MyArgvFlag mkconfig = {
    .value = {0},
    .description = "Rewrites the config.mk template, asks for confirmation",
    .longName = "mkconfig",
    .shortName = 0,
    .expectValue = false,
    .trigged = false
};

MyArgvFlag mktargets = {
    .value = {0},
    .description = "Rewrites the target.mk template, asks for confirmation",
    .longName = "mktargets",
    .shortName = 0,
    .expectValue = false,
    .trigged = false
};

MyArgvFlag clangd = {
    .value = {0},
    .description = "Rewrites the .clangd template, asks for confirmation",
    .longName = "clangd",
    .shortName = 0,
    .expectValue = false,
    .trigged = false
};

MyArgvFlag settings = {
    .value = {0},
    .description = "Rewrites the .vscode/settings.json template, asks for confirmation",
    .longName = "settings",
    .shortName = 0,
    .expectValue = false,
    .trigged = false
};

MyArgvFlag gitignore = {
    .value = {0},
    .description = "Rewrites the .gitignore template, asks for confirmation",
    .longName = "gitignore",
    .shortName = 0,
    .expectValue = false,
    .trigged = false
};

MyArgvFlag githubPublic = {
    .value = {0},
    .description = "Creates a public github repository with the name of the project, depends on gh command (github cli)",
    .longName = "github",
    .shortName = 'g',
    .expectValue = false,
    .trigged = false
};

MyArgvFlag githubPrivate = {
    .value = {0},
    .description = "Creates a private github repository with the name of the project, depends on gh command (github cli)",
    .longName = "github-private",
    .shortName = 0,
    .expectValue = false,
    .trigged = false
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