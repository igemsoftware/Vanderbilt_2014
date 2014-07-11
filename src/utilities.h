#ifndef ___UTILITIES_H___
#define ___UTILITIES_H___

#include <stdio.h>					// provide file, print facilities to files
#include <stdbool.h>				// provide booleans to other files
#include <stdlib.h>					// for malloc
#include <string.h>					// for strlen/strcpy/strcat
#include <ctype.h>					// for toupper

// MACROS

#define PRINT_ERROR(str) fprintf(stderr,"%s",str)

#define PRINT_ERROR_AND_RETURN_NULL_IF_NULL(ptr,str)			\
	if (NULL == ptr){																				\
	fprintf(stderr,"%s",str);																\
	return NULL;																						\
	}

#define PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NULL(ptr,str)	\
	if (NULL == ptr){																			\
	fprintf(stderr,"%s",str);															\
	return -1;																						\
	}

#define PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NEG_ONE(val,str)	\
	if (-1 == val){																						\
	fprintf(stderr,"%s",str);																	\
	return -1;																								\
	}

#define PRINT_ERROR_AND_RETURN_NULL_IF_NEG_ONE(val,str)	\
	if (-1 == val){																				\
	fprintf(stderr,"%s",str);															\
	return NULL;																					\
	}


// STRUCTS

// used to return a char string, along with size information
typedef struct{
	char * string;
	size_t cur_size;							// current number of useful bytes this is storing
	size_t full_size;							// full size of char * in bytes
} string_with_size;							// NOT null-terminated by default!

typedef struct{
	size_t begin_index;
	size_t end_index;
} dna_reading_indices;

#endif /*___UTILITIES_H___*/
