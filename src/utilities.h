#ifndef ___UTILITIES_H___
#define ___UTILITIES_H___

/*
    preprocessor macros, system headers, and other generic additions to make
    extending this program easier by cataloging all nonstandard utilities in
    one spot
*/

// HEADERS AND LIBRARIES

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>  // for malloc
#include <string.h>  // for strlen/strcpy/strcat
#include <glib.h>    // for queues, lists, and threads
#include <gmp.h>     // for bignums

// MACROS

#define PRINT_ERROR_NO_NEWLINE(str) fprintf(stderr, "%s", str)

#define PRINT_ERROR_WITH_NEWLINE(str) fprintf(stderr, "%s\n", str)

#define PRINT_ERROR_NEWLINE() fprintf(stderr, "\n")

#define PRINT_ERROR_SIZE_T_NO_NEWLINE(num) fprintf(stderr, "%zu", num)

#define PRINT_ERROR_AND_RETURN_IF_NULL(ptr, str) \
    if (NULL == ptr) {                           \
        PRINT_ERROR_WITH_NEWLINE(str);           \
        return;                                  \
    }

// FUNCTIONS

// open file, return pointer
/**
 * @brief:
 *
 */
static inline FILE * open_file_read(char * filename) {
    FILE * input_file = fopen(filename, "r");
    return input_file;
}
// TODO: mention truncation
static inline FILE * create_file_binary_write(char * filename) {
    FILE * output_file = fopen(filename, "wb");
    return output_file;
}
// create file, return pointer

// STRUCTS AND FUNCTIONS TO MANIPULATE THEM
// STRING_WITH_SIZE
// used to return a char string, along with size information
// TODO: mention that readable_bytes is used by functions like fread because
// while they will typically fill the entire memory space sometimes they do
// less, upon reaching EOF or some other
typedef struct {
    char * string;
    size_t readable_bytes; // current number of useful bytes this is storing
    size_t size_in_memory; // full size of char * in bytes
} string_with_size;        // NOT null-terminated by default!
// TODO: javadoc
// note that sets readable_bytes to 0
static inline string_with_size *
  make_new_string_with_size(size_t size_in_memory) {
    string_with_size * sws_to_return = malloc(sizeof(string_with_size));
    sws_to_return->string = malloc(size_in_memory * (sizeof(char)));
    sws_to_return->readable_bytes = 0;
    sws_to_return->size_in_memory = size_in_memory;
    return sws_to_return;
}
// TODO: javadoc
static inline string_with_size *
  set_string_with_size_readable_bytes(string_with_size * sws,
                                      size_t readable_bytes) {
    sws->readable_bytes = readable_bytes;
    return sws;
}
// TODO: javadoc
static inline void free_string_with_size(string_with_size * sws_to_free) {
    free(sws_to_free->string);
    free(sws_to_free);
}

#endif /*___UTILITIES_H___*/
