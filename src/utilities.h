#ifndef ___UTILITIES_H___
#define ___UTILITIES_H___

// description
/*
    preprocessor macros, system headers, and other generic additions to make
    extending this program easier by cataloging all nonstandard utilities in
    one spot
*/

// searchable comment syntax used throughout project
/*
    IFFY: could cause errors, check if weird things are happening
    TODO: do this now
    OPTIMIZATION: if speed needed in this function, check here first
*/

// HEADERS AND LIBRARIES

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> // for malloc
#include <string.h> // for strlen/strcpy/strcat
#include <glib.h>   // for queues, lists, and threads
#include <gmp.h>    // for bignums

// PORTABILITY MACROS
// IFFY: this could cause issues with processing files not made on linux systems
#define NEWLINE '\n'

// MACROS
// extra parens used around all macro arguments for evaluation before calling

#define PRINT_ERROR_NO_NEWLINE(str) fprintf(stderr, "%s", (str))

#define PRINT_ERROR(str) fprintf(stderr, "%s\n", (str))

#define PRINT_ERROR_NEWLINE() fprintf(stderr, "\n")

#define PRINT_ERROR_SIZE_T_NO_NEWLINE(num) fprintf(stderr, "%zu", (num))

#define PRINT_ERROR_UNSIGNED_LONG_LONG_NO_NEWLINE(num) \
    fprintf(stderr, "%llu", (num))

#define PRINT_ERROR_MPZ_T_NO_NEWLINE(bignum) mpz_out_str(stderr, 10, (bignum))

#define PRINT_ERROR_AND_RETURN_IF_NULL(ptr, str) \
    if (NULL == (ptr)) {                         \
        PRINT_ERROR((str));                      \
        return;                                  \
    }

#define PRINT_ERROR_DIVIDER_LINE() fprintf(stderr, "%s\n", "------------");

#define PRINT_ERROR_STRING_FIXED_LENGTH_NO_NEWLINE(str, len) \
    fprintf(stderr, "%.*s", (int)len, str)

// x is vertical (downwards), y is horizontal (rightwards)
// parentheses required because of distributivity
#define TWO_D_ARRAY_INDEX(arr, x, y, max_y) (arr)[(x) * (max_y) + (y)]

// FUNCTIONS
// FILE FUNCTIONS

/**
 * @brief:
 * ...
 */
FILE * open_file_read(const char * filename);
// TODO: mention truncation
FILE * create_file_binary_write(const char * filename);

// STRUCTS AND FUNCTIONS TO MANIPULATE THEM
// STRING_WITH_SIZE
// used to return a char string, along with size information
// TODO: mention that readable_bytes is used by functions like fread because
// while they will typically fill the entire memory space sometimes they do
// less, upon reaching EOF or some other ferror
typedef struct {
    char * string; // NOT null-terminated by default!
    unsigned long long
      readable_bytes; // current number of useful bytes this is storing
    unsigned long long size_in_memory; // full size of char * in bytes
} string_with_size;
// TODO: javadoc
// note that sets readable_bytes to 0
string_with_size * make_new_string_with_size(unsigned long long size_in_memory);
#ifdef DEBUG
// DOES NOT COPY OVER NULL CHAR TERMINATING
string_with_size * make_new_string_with_size_given_string(char * null_term_str);
#endif
// TODO: javadoc
string_with_size *
  set_string_with_size_readable_bytes(string_with_size * sws,
                                      unsigned long long readable_bytes);
// TODO: javadoc
string_with_size * copy_string_with_size(string_with_size * from_sws,
                                         string_with_size * to_sws);
// TODO: javadoc
string_with_size * grow_string_with_size(string_with_size ** sws,
                                         unsigned long long final_size_in_mem);
// TODO: javadoc
void free_string_with_size(void * arg);

#endif /*___UTILITIES_H___*/
