#ifndef ___UTILITIES_H___
#define ___UTILITIES_H___

#include <stdio.h>					// provide file, print facilities to files
#include <stdbool.h>				// provide booleans to other files
#include <stdlib.h>					// for malloc
#include <string.h>					// for strlen/strcpy/strcat
#include <ctype.h>					// for toupper
#include <glib.h>						// for GList
#include <gmp.h>						// for mpz_t

// MACROS

#define PRINT_ERROR(str) fprintf(stderr,"%s\n",str)

#define PRINT_ERROR_AND_RETURN_NULL_IF_NULL(ptr,str)			\
	if (NULL == ptr){																				\
	PRINT_ERROR(str);																				\
	return NULL;																						\
	}

#define PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NULL(ptr,str)	\
	if (NULL == ptr){																			\
	PRINT_ERROR(str);																			\
	return -1;																						\
	}

#define PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NEG_ONE(val,str)	\
	if (-1 == val){																						\
	PRINT_ERROR(str);																					\
	return -1;																								\
	}

#define PRINT_ERROR_AND_RETURN_NULL_IF_NEG_ONE(val,str)	\
	if (-1 == val){																				\
	PRINT_ERROR(str);																			\
	return NULL;																					\
	}

#define PRINT_ERROR_AND_PERFORM_EXPR_IF_NULL(val,str,expr)	\
	if (NULL == val){																					\
	PRINT_ERROR(str);																					\
	expr;																											\
	}

#define PRINT_ERROR_AND_PERFORM_EXPR_IF_NEG_ONE(val,str,expr)	\
	if (-1 == val){																							\
	PRINT_ERROR(str);																						\
	expr; 																											\
	}

#define PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NULL(val,str,expr,retval)	\
	if (NULL == val){ 																													\
	PRINT_ERROR(str); 																													\
	expr; 																																			\
	return retval; 																															\
	}

#define PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NEG_ONE(val,str,expr,retval)	\
	if (-1 == val){ 																															\
	PRINT_ERROR(str); 																														\
	expr; 																																				\
	return retval; 																																\
	}
	

// STRUCTS

// used to return a char string, along with size information
typedef struct{
	char * string;
	size_t cur_size;							// current number of useful bytes this is storing
	size_t full_size;							// full size of char * in bytes
} string_with_size;							// NOT null-terminated by default!

inline string_with_size * make_new_string_with_size_ptr (size_t full_size){
	string_with_size * sws_to_return = (string_with_size *) malloc(sizeof(string_with_size));
	sws_to_return->string = (char *) malloc(full_size*(sizeof(char)));
	sws_to_return->cur_size = 0;
	sws_to_return->full_size = full_size;
	return sws_to_return;
}

inline void free_string_with_size_ptr(string_with_size * sws_to_free){
	free(sws_to_free->string);
	free(sws_to_free);
}

typedef struct{
	size_t begin_index;
	size_t end_index;
} dna_reading_indices;

#endif /*___UTILITIES_H___*/
