/** @author Nicolas Boeckh & Sofia Delijaj */
#include <fcntl.h>      /* File I/O */
#include <stdio.h>
#include <unistd.h>     /* ssize_t*/
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>

#include "directoryUtilities.h"

/** Will create a directory and it's parents until attaining a directory that exists (recursive)
 * @param path  The path to the directory
 * @param permissions The permissions this directory should enjoy
 */ 
int createIfNotExists(const char* path, __mode_t permissions) {
    struct stat s;
    errno = 0;
    int err = stat(path, &s);
    char *errMsg = (char*) malloc(THEORETICAL_MAX_PATH_LENGTH + 100);
    
    if(err == -1) {     // If stat failed
        if(errno == ENOENT) {   // The directory at path does not exist.
            errno = 0;
            mkdir(path, permissions);
        }

        if (errno == ENOENT) {  // If it couldn't create the directory, because it's parent does not exist.
            char *upperDir = (char*) malloc(THEORETICAL_MAX_PATH_LENGTH);
            upperDir = dirname(strdup(path));           // 1 - Get the path to it's parent
            createIfNotExists(upperDir, permissions);   // 2 - Create the parent (recursion)
            mkdir(path, permissions);                   // 3 - Create itself
            free(upperDir);
        } else if (errno > 0) {
            snprintf(errMsg, THEORETICAL_MAX_PATH_LENGTH + 100, "FILE or DIR at %s throws %s", path, strerror(errno));
            FAIL_WITH_PASSION({ free(errMsg); }, errMsg, errno);        // Macro { @see directoryUtilities.h }
        }
    } // Otherwise the folder exists

    free(errMsg);
    return (errno == 0) ? 1 : 0;
}

/** Will get the permissions of the given file __mode_t
 * @param mode The mode of the stat associated with the node
 */
int getPermissions(__mode_t mode) {
    // Convert the permissions to an S_IRWXU-S_IRWXG-S_IRWXO binary number 
    return  (mode & S_IRUSR) << 8 | (mode & S_IWUSR) << 7 | (mode & S_IXUSR) << 6 | \
            (mode & S_IRGRP) << 5 | (mode & S_IWGRP) << 4 | (mode & S_IXGRP) << 3 | \
            (mode & S_IROTH) << 2 | (mode & S_IWOTH) << 1 | (mode & S_IXOTH);
}

/** Will copy a file from a given source to a given destination, with certain additional behaviors.
 * @param src The source Object (struct defined at ./directoryUtilities.h)
 * @param dest The path to the expected destination.
 * @param modifyRights Whether (if the file already exists, the rights should be modified)
 */
int copyFile(Object *src, const char *dest, int modifyRights) {
    printf("cp\t\t%40s -> %-40s", src->path[0], dest);  // Fun print functions everywhere.

    int fd_src, fd_dest;        // The original and destination file pointers (I think ?) 
    char buffer[BUFFER_SIZE];   // A buffer to store copied chars
    ssize_t n_read;             // The amount of bytes read per iteration (max BUFFER_SIZE = 4096)
    errno = 0;

    fd_src = open(src->path[1], O_RDONLY);  // Open the source file in read-only mode
    if (errno > 0) {
        WARN_CUSTOM("\rCould not open source %s: %s\n", src->path[0], strerror(errno)); // Macro { @see directoryUtilities.h }
        return -1;
    }

    int permissions = getPermissions(src->info.st_mode);        // Get the permissions for the source file (useful that we store stat's)
    fd_dest = open(dest, O_WRONLY | O_CREAT | O_EXCL, permissions);
    // If the destination file exists, we should overwrite it only if the destination is older, 
    // and we should only copy the rights if they have changed and this was requested as option.
    if (errno == EEXIST) {
        errno = 0;
        struct stat *destInfo = malloc(sizeof(struct stat));    // Get information concerning the destination file.
        int lstatus = lstat(dest, destInfo);
        if (lstatus == 0) { // The file exists
            // If the destination is older (time of modification in secs since Epoch), then overwrite (O_TRUNC)
            // Rights are set anyways on overwrite.
            if (destInfo->st_mtime < src->info.st_mtime)
                fd_dest = open(dest, O_WRONLY | O_TRUNC, permissions);
            else if (modifyRights == 1) { // Or if the destination is newer and we've specified the modification of rights
                printf("\rchmod %35s", dest);
                chmod(dest, src->info.st_mode);
                return 0;
            } else { // No need for copy
                printf("\tNot necessary\n");
                return 0;
            }
        }
        if (lstatus != 0 || errno > 0) {    // If there is an error or a stat error (more often than not it's E)
            WARN_CUSTOM("\rOverwriting %s throws %s\n", dest, (errno > 0) ? strerror(errno) : "stat error");
            close(fd_src);
            return -1;
        }
    } else if (errno > 0) { // If the destination can't be written to for some reason.
        WARN_CUSTOM("\rCould not open dest %s: %s\n", dest, strerror(errno));
        close(fd_src);
        return -1;
    }

    int size = src->info.st_size;   // QOL stuff, allows to make a progress bar.
    int cur = 0;                    // Don't mention that it's unnecessary (hi Rose !), it's just plain flashy me.

    while (n_read = read(fd_src, buffer, sizeof buffer), n_read > 0) {  // While bytes can be read from the src file.
        char *out = buffer;    
        ssize_t bytes_written;

        do {    // Write bytes to the destination file.
            bytes_written = write(fd_dest, out, n_read);
            if (bytes_written >= 0) {   // So long as bytes were written.
                n_read -= bytes_written;
                out += bytes_written;
                cur += bytes_written;
                printf("\rcp\t    %35s -> %-35s %12d/%-12d", src->path[0], dest, cur, size);
            } else if (errno != EINTR) {    // Fail because copy was interrupted.
                WARN_CUSTOM("\rCould not copy %s to %s : %s because no\n", src->path[0], dest, strerror(errno)); // https://www.reddit.com/r/softwaregore/comments/cjwa5z/could_not_copy_files_because_no/
                close(fd_src);
                close(fd_dest);
                return -1;
            }
        } while (n_read > 0);
    }

    if (n_read != 0) { // If some bytes could not be written.
        WARN_CUSTOM("\rCould not copy %s to %s : %s\n", src->path[0], dest, strerror(errno));
        close(fd_src);
        close(fd_dest);
        return -1;
    }

    printf("\rcp\t    %35s -> %-35s    Done %12d bytes\n", src->path[0], dest, size); // QOL (sue me !)
    close(fd_src);
    close(fd_dest);
    return 0;
}

/** Copy a node (struct defined at ./directoryUtilities.h) to a specified destination
 *  @param node         The source Object (struct defined at ./directoryUtilities.h)
 *  @param destination  The destination path.
 *  @param copyLinks    Whether the program should copy links instead of dereferencing them.
 *  @param modifyRights Whether the program should overwrite permissions.
 */ 
void copyNode(Object *node, char destination[THEORETICAL_MAX_PATH_LENGTH], int copyLinks, int modifyRights) {
    if (node->symlink == 1 && copyLinks == 1) { // Should make symlink instead of dereferencing. (can't test for S_ISLNK(node->info.st_mode)
        errno = 0;                              // because default stat on realpath when symlink).
        symlink(node->path[1], destination);
        if (errno > 0)
            WARN_CUSTOM("Could not link %s to %s because %s", node->path[1], destination, strerror(errno));
    } else if (S_ISDIR(node->info.st_mode) == 1) {      // If the node represents a directory
        printf("mkdir %40s -> %-40s\n", node->path[1], destination);    // Snazzy prints.
        createIfNotExists(destination, node->info.st_mode);             // Make the destination dir.
        for (int i = 1; i < node->numberOfChildren + 1; i++) {          // For all children call this method recursively (1 -> n + 1 because children[0] is parent (it makes sense in my mind))
            char* destinationChild = malloc(THEORETICAL_MAX_PATH_LENGTH);
            snprintf(destinationChild, THEORETICAL_MAX_PATH_LENGTH, "%s/%s", destination, basename(((Object*) node->children[i])->path[0]));    // Path to child at destination.
            copyNode((Object*) node->children[i], destinationChild, copyLinks, modifyRights);   // Copy the child node (recursion galore).
            free(destinationChild);
        }
    } else if (S_ISREG(node->info.st_mode)) {   // If it's a file, just copy it, no more, no less.
        copyFile(node, destination, modifyRights);
    }
}
