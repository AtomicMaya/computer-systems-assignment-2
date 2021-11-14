/** @author Nicolas Boeckh & Sofia Delijaj */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h> 
#include <libgen.h>

#include "directoryRunner.h"
#include "directoryPrinter.h"
#include "directoryUtilities.h"
#include "directoryCopier.h"
#include "argsHandler.h"

/** Let's do this one last time, my __name__ == '__main__' (#IntoTheSpiderverse)
 * @param argc The number of program arguments
 * @param argv The given arguments.
 */ 
int main(int argc, char **argv) {
    int behavior = getBehavior(argc, argv); // Get the optional behavior possibly asked of the program.
    char *destination = malloc(THEORETICAL_MAX_PATH_LENGTH);
    char **sources = malloc((argc - 2) * THEORETICAL_MAX_PATH_LENGTH);

    switch (argc) {
        case 1: // (== Error case)
            fprintf(stderr, "Usage: %s file\n", argv[0]);
            exit(EXIT_FAILURE);
            break;
        case 2: // (== List)
            stripSlash(argv[argc - 1], destination);       // Remove crazy "/" at the end.
            Object* obj = createNode(destination, NULL);   // Create the node.
            printObject(obj);                              // Print it.
            freeNode(obj);                                 // Drop it. 
            break;
        
        default: // In theory (== copy) (arguments)
            stripSlash(argv[argc - 1], destination); // Remove crazy "/" at the end.

            for (int i = optind; i < argc; i++) {   // Get all possible destinations.
                if (strcmp(argv[i], destination)) {
                    sources[i - optind] = malloc(THEORETICAL_MAX_PATH_LENGTH);
                    snprintf(sources[i - optind], THEORETICAL_MAX_PATH_LENGTH, "%s", argv[i]);
                }
            }

            if (argc - 1 - optind == 0) // Failing with passion, a macro meaning color and stuff to be executed, before crashing the Matrix or something.
                FAIL_WITH_PASSION({ free(destination); }, "Insufficient/Invalid arguments\nUsage : \n./ultra_cp [file|folder]\nOR\n./ultra_cp sources [...] destination [-af]", EINVAL);

            for (int i = 0; i < argc - 1 - optind; i++) {   // Copy each source to destination.
                Object* obj = createNode(sources[i], NULL); // Create the relevant node.
                char *nodeDestination = malloc(THEORETICAL_MAX_PATH_LENGTH);
                snprintf(nodeDestination, THEORETICAL_MAX_PATH_LENGTH, "%s/%s", destination, basename(sources[i])); // Set the destination path.
                copyNode(obj, nodeDestination, (behavior & COPY_LINKS) >> 1, behavior & MODIFY_RIGHTS); // Copy it.
                freeNode(obj); // Drop it.
                free(nodeDestination);
                free(sources[i]);
            }
            break;
    }
    free(destination);
    free(sources);
    
    return 0;
}