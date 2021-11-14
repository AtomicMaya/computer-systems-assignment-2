/** @author Nicolas Boeckh */
#ifndef HEADER_UTILITIES
#define HEADER_UTILITIES
#define THEORETICAL_MAX_PATH_LENGTH 4096
#define BUFFER_SIZE                 4096

#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define FAIL(errName, err)                      { printColorized((errName), ANSI_FGCOLOR_RED, ANSI_BGCOLOR_BLACK, ANSI_DECO_NONE, 1);  exit((err)); }
#define FAIL_WITH_PASSION(bloc, errStr, err)    { printColorized((errStr), ANSI_FGCOLOR_RED, ANSI_BGCOLOR_BLACK, ANSI_DECO_NONE, 1); bloc; exit((err)); }
#define WARN(errMsg)                            { printColorized((errMsg), ANSI_FGCOLOR_RED, ANSI_BGCOLOR_BLACK, ANSI_DECO_NONE, 1); }
#define WARN_CUSTOM(fmt, ...)                   { char *errMsg = malloc(BUFFER_SIZE + 100); snprintf(errMsg, BUFFER_SIZE + 100, fmt, __VA_ARGS__); WARN(errMsg); free(errMsg); }

#define ANSI_FGCOLOR_BLACK      30
#define ANSI_FGCOLOR_RED        31
#define ANSI_FGCOLOR_GREEN      32
#define ANSI_FGCOLOR_YELLOW     33
#define ANSI_FGCOLOR_BLUE       34
#define ANSI_FGCOLOR_MAGENTA    35
#define ANSI_FGCOLOR_CYAN       36
#define ANSI_FGCOLOR_WHITE      37
#define ANSI_FGCOLOR_BLACK_B    90
#define ANSI_FGCOLOR_RED_B      91
#define ANSI_FGCOLOR_GREEN_B    92
#define ANSI_FGCOLOR_YELLOW_B   93
#define ANSI_FGCOLOR_BLUE_B     94
#define ANSI_FGCOLOR_MAGENTA_B  95
#define ANSI_FGCOLOR_CYAN_B     96
#define ANSI_FGCOLOR_WHITE_B    97

#define ANSI_BGCOLOR_BLACK      40
#define ANSI_BGCOLOR_RED        41
#define ANSI_BGCOLOR_GREEN      42
#define ANSI_BGCOLOR_YELLOW     43
#define ANSI_BGCOLOR_BLUE       44
#define ANSI_BGCOLOR_MAGENTA    45
#define ANSI_BGCOLOR_CYAN       46
#define ANSI_BGCOLOR_WHITE      47
#define ANSI_BGCOLOR_BLACK_B    40
#define ANSI_BGCOLOR_RED_B     101
#define ANSI_BGCOLOR_GREEN_B   102
#define ANSI_BGCOLOR_YELLOW_B  103
#define ANSI_BGCOLOR_BLUE_B    104
#define ANSI_BGCOLOR_MAGENTA_B 105
#define ANSI_BGCOLOR_CYAN_B    106
#define ANSI_BGCOLOR_WHITE_B   107

#define ANSI_DECO_NONE         0
#define ANSI_DECO_BOLD         1
#define ANSI_DECO_FAINT        2
#define ANSI_DECO_ITALIC       3
#define ANSI_DECO_UNDERLINE    4

typedef struct {
    int numberOfChildren;
    int symlink;
    int cyclic;
    char path[2][THEORETICAL_MAX_PATH_LENGTH];
    struct stat info;
    struct Object **children;
} Object;

void stripSlash(char arg[], char *filename);

void printColorized(char *string, int ANSI_FGCOLOR, int ANSI_BGCOLOR, int ANSI_DECO, int newLine);

#endif