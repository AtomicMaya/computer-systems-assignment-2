/** @author Nicolas Boeckh & Sofia Delijaj */
#ifndef HEADER_RUNNER
#define HEADER_RUNNER  // Linux max path length. Win is 260

#include "directoryUtilities.h"

void freeNode(Object *node);

Object * createNode(const char *path, Object *parent);

#endif