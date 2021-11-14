/** @author Nicolas Boeckh & Sofia Delijaj */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "directoryUtilities.h"

/** Generate a type/permission string from a given __mode_t
 * @param permissions   A char buffer this should be appended to.
 * @param st_mode       The stat info of the file (at realpath), which is why
 * @param isSymlink     we need this : can't test for S_ISLNK(node->info.st_mode).
 */
void generatePermissions(char *permissions, __mode_t st_mode, int isSymlink) {
    snprintf(permissions, 10, "%s%s%s%s%s%s%s%s%s%s", (isSymlink == 1) ? "l" : ((S_ISDIR(st_mode)) ? "d" : "-"),
        (st_mode & S_IRUSR) ? "r" : "-", (st_mode & S_IWUSR) ? "w" : "-", (st_mode & S_IXUSR) ? "x" : "-",
        (st_mode & S_IRGRP) ? "r" : "-", (st_mode & S_IWGRP) ? "w" : "-", (st_mode & S_IXGRP) ? "x" : "-",
        (st_mode & S_IROTH) ? "r" : "-", (st_mode & S_IWOTH) ? "w" : "-", (st_mode & S_IXOTH) ? "x" : "-");
}

/** Print the object, kind of like an ultra-li
 * @param node An Object (struct defined at ./directoryUtilities.h) representing a file/dir/link in a file tree.
 */
void printObject(Object *node) {
    if (node->info.st_ctime == 0) {     // If the node just doesn't exist.
        printColorized("ERROR, DOES NOT EXIST", ANSI_FGCOLOR_RED, ANSI_BGCOLOR_YELLOW, ANSI_DECO_BOLD, 1);
        return;
    }

    char *permissions = malloc(sizeof(char) * 11);
    generatePermissions(permissions, node->info.st_mode, node->symlink);    // Get the permissions string

    char *date = asctime(localtime(&node->info.st_mtime));  // Convert the acquired date to something digestible.
    date[strlen(date) - 1] = '\0';
    
    printf("%s\t%*ld\t%s\t", permissions, 12, node->info.st_size, date);    // Pretty print (assumes file < 999.999'999'999 GB, an assumption to be sure, but a safe one)
    if (node->symlink == 1) {   // If it's a symlink
        if (node->cyclic == 1)  // If for some reason some id*#t made a symlink that points to higher in the file tree
            printColorized("(!!! CYCLIC !!!) -> ", ANSI_FGCOLOR_RED, ANSI_BGCOLOR_BLACK, ANSI_DECO_BOLD, 0);    // Yay, colors !
        printColorized(node->path[0], ANSI_FGCOLOR_CYAN_B, ANSI_BGCOLOR_BLACK, ANSI_DECO_NONE, 0);
        printf(" %s ", "->");
        printColorized(node->path[1], ANSI_FGCOLOR_BLUE_B, ANSI_BGCOLOR_BLACK, ANSI_DECO_NONE, 1);
    }
    else if (S_ISDIR(node->info.st_mode)) // Yay, different colors !
        printColorized(node->path[0], ANSI_FGCOLOR_BLUE, ANSI_BGCOLOR_GREEN, ANSI_DECO_NONE, 1);
    else // Yaaaay, even more colors... this is getting old quick...
        printColorized(node->path[0], ANSI_FGCOLOR_GREEN, ANSI_BGCOLOR_BLACK, ANSI_DECO_NONE, 1);
    
    free(permissions);
    for (int i = 1; i < node->numberOfChildren + 1; i++)
        printObject((Object*) node->children[i]);   // Print children, recursively (because we have recursion everywhere else, why not here ?)
}