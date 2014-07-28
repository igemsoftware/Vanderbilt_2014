#ifndef ___UTILITIES_H___
#define ___UTILITIES_H___

/*
	preprocessor macros, system headers, and other generic additions to make
	extending this program easier by cataloging all nonstandard utilities in
	one spot
*/

// obviously if a system header needs to be added (changing this filee) then
// having everything in one file means the whole thing needs to be recompiled
// however, in a project like this where full compilation takes a single minute
// this won't be an issue, and the convenience outweights any issues'

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

// gmp bignums used for keeping track of large (genome-scale) byte/line numbers
typedef struct{
	int result;
	mpz_t number_processed;
} result_number_processed;
// CTOR
// uses pointers so potentially massive mpz_t doesn't have to be copied by value'
result_number_processed * initialize_result_number_processed_ptr(result_number_processed * rbp);
// DTOR
void free_result_number_processed_ptr(result_number_processed * rbp);
// METHODS
inline result_number_processed * add_to_number_processed_ptr(result_number_processed * rbp, unsigned long int added_bytes){
	mpz_add_ui(rbp->number_processed,rbp->number_processed,added_bytes);
	return rbp;
}
inline result_number_processed * increment_number_processed_ptr(result_number_processed * rbp){
	add_to_number_processed_ptr(rbp,1);
	return rbp;
}
void print_number_processed_ptr(result_number_processed * rbp, FILE * outstream);
typedef struct{
	result_number_processed * bytes_read;
	result_number_processed * bytes_written;
} result_number_processed_pair;
void free_result_number_processed_pair(result_number_processed_pair rbpp);

#endif /*___UTILITIES_H___*/
