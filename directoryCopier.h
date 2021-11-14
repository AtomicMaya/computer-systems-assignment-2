/** @author Nicolas Boeckh & Sofia Delijaj */
#ifndef HEADER_COPIER
#define HEADER_COPIER

void copyNode(Object *origin, char destination[THEORETICAL_MAX_PATH_LENGTH], int copyLinks, int modifyBehavior);

#endif