/** @author Nicolas Boeckh & Sofia Delijaj */
#include <getopt.h>
#include <errno.h>

#include "argsHandler.h"
#include "directoryUtilities.h"

/** Gets the targeted behavior of the program as a binary number where any x_i represents the state of a unique parameter.
 * @param argc The number of arguments given to the program (untouched from original)
 * @param argv The arguments themselves.
 */
int getBehavior(int argc, char *argv[]) {
    char opt;

    int copyLinks = 0b00;   // Initialize both values to zero to ease return.
    int modifyRights = 0b00;

    while ((opt = getopt(argc, argv, "af")) != -1) {    // So long as options where given (matching either/and a, f)
        switch(opt) {
            case 'a':   // -a represents that the program should be able to override file permissions.
                modifyRights = MODIFY_RIGHTS;
                break;
            case 'f':                   // -f represents that symlinks should not be dereferenced but should be 
                copyLinks = COPY_LINKS; // recreated at the destination to point to the original destination.
                break;
            default:                    // If it is anything else, the program should exit, with failure.
                errno = EINVAL;
                FAIL_WITH_PASSION({ }, strerror(errno), errno);     // Macro { @see directoryUtilities.h }
                break;
        }
    }
    return copyLinks | modifyRights; // Return the binary OR of both values, resulting in a value between 0 and 3.
}