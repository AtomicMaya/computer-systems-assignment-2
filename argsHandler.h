/** @author Nicolas Boeckh & Sofia Delijaj */
#ifndef HEADER_ARGS
#define HEADER_ARGS

#define MODIFY_RIGHTS   0b01    // Use as a mask for binary numbers, allows to use one int to define 
#define COPY_LINKS      0b10    // multiple behaviors.

/** Gets the targeted behavior of the program as a binary number where any x_i represents the state of a unique parameter.
 * @param argc The number of arguments given to the program (untouched from original)
 * @param argv The arguments themselves.
 */
int getBehavior(int argc, char* argv[]);

#endif