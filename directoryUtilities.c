/** @author Nicolas Boeckh & Sofia Delijaj */
#include <string.h>
#include <stdio.h>

#include "directoryUtilities.h"

/** Removes the final "/" from a given argument (to avoid accidental // in paths).
 * @param arg Typically a given argument at program launch.
 * @param filename A result buffer.
 */ 
void stripSlash(char arg[], char *filename) {
    strncpy(filename, arg, THEORETICAL_MAX_PATH_LENGTH);
    while (filename[strlen(filename) - 1] == '/')   // So long as there are "/"
        filename[strlen(filename) - 1] = '\0';      // Demonetize them (CC-BY CaptainSparklez)
}

/** Allows fancy color printouts to the console.
 * @param string    The string to funky print
 * @param ANSI_FG_COLOR A foreground ANSI setting
 * @param ANSI_BG_COLOR A background ANSI setting
 * @param ANSI_DECO A decoration (underline, bold, etc.) ANSI setting
 * @param newline Whether a newline should be printed at the end.
 */ 
void printColorized(char *string, int ANSI_FGCOLOR, int ANSI_BGCOLOR, int ANSI_DECO, int newLine) {
    printf("\033[%i;%i;%im%s\033[0m%s", ANSI_DECO, ANSI_BGCOLOR, ANSI_FGCOLOR, string, (newLine == 1) ? "\n\0" : "\0");
}