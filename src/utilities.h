#ifndef ___UTILITIES_H___
#define ___UTILITIES_H___

/*
	preprocessor macros, system headers, and other generic additions to make
	extending this program easier by cataloging all nonstandard utilities in
	one spot
*/

// HEADERS AND LIBRARIES

#include <stdio.h>					// provide file, print facilities to files
#include <stdbool.h>				// provide booleans to other files
#include <stdlib.h>					// for malloc
#include <string.h>					// for strlen/strcpy/strcat
#include <glib.h>						// for GSList
#include <gmp.h>						// for mpz_t

// MACROS

#define PRINT_ERROR(str) fprintf(stderr, "%s\n", str)

#define PRINT_ERROR_AND_RETURN_NULL_IF_NULL(ptr,str)		\
	if (NULL == ptr){																				\
	PRINT_ERROR(str);																				\
	return NULL;																						\
	}

#define PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NULL(ptr,str)	\
	if (NULL == ptr){																			\
	PRINT_ERROR(str);																				\
	return -1;																						\
	}

#define PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NEG_ONE(val,str)	\
	if (-1 == val){																						\
	PRINT_ERROR(str);																						\
	return -1;																								\
	}

#define PRINT_ERROR_AND_RETURN_NULL_IF_NEG_ONE(val,str)	\
	if (-1 == val){																				\
	PRINT_ERROR(str);																				\
	return NULL;																					\
	}

#define PRINT_ERROR_AND_PERFORM_EXPR_IF_NULL(val,str,expr)	\
	if (NULL == val){																					\
	PRINT_ERROR(str);																						\
	expr;																											\
	}

#define PRINT_ERROR_AND_PERFORM_EXPR_IF_NEG_ONE(val,str,expr)	\
	if (-1 == val){																							\
	PRINT_ERROR(str);																							\
	expr; 																											\
	}

#define PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NULL(val,str,expr,retval)	\
	if (NULL == val){ 																													\
	PRINT_ERROR(str); 																														\
	expr; 																																			\
	return retval; 																															\
	}

#define PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NEG_ONE(val,str,expr,retval)	\
	if (-1 == val){ 																															\
	PRINT_ERROR(str); 																															\
	expr; 																																				\
	return retval; 																																\
	}

// FUNCTIONS

// open file, return pointer
FILE * open_file_read (char * filename);
FILE * create_file_binary_write (char * filename); // create file, return pointer

// STRUCTS AND FUNCTIONS TO MANIPULATE THEM

// used to return a char string, along with size information
typedef struct{
	char * string;
	size_t readable_bytes; // current number of useful bytes this is storing
	size_t size_in_memory; // full size of char * in bytes
} string_with_size;			 // NOT null-terminated by default!

inline string_with_size * make_new_string_with_size (size_t size_in_memory){
	string_with_size * sws_to_return = malloc(sizeof(string_with_size));
	sws_to_return->string = malloc(size_in_memory * (sizeof(char)));
	sws_to_return->readable_bytes = 0;
	sws_to_return->size_in_memory = size_in_memory;
	return sws_to_return;
}

inline void free_string_with_size (string_with_size * sws_to_free){
	free(sws_to_free->string);
	free(sws_to_free);
}

typedef struct{
	size_t begin_index;
	size_t end_index;
} dna_reading_indices;

// gmp bignums used for keeping track of large (genome-scale) byte numbers
typedef struct{
	int result;
	mpz_t bytes_processed;
} result_bytes_processed;
// CTOR
// uses pointers so potentially massive mpz_t doesn't have to be copied by value'
result_bytes_processed * initialize_result_bytes_processed (result_bytes_processed * rbp);
// DTOR
void free_result_bytes_processed (result_bytes_processed * rbp);
// METHODS
inline result_bytes_processed * add_to_bytes_processed (result_bytes_processed * rbp,
																												unsigned long int added_bytes){
	mpz_add_ui(rbp->bytes_processed,
						 rbp->bytes_processed,
						 added_bytes);
	return rbp;
}
inline result_bytes_processed * increment_bytes_processed (result_bytes_processed * rbp){
	return add_to_bytes_processed(rbp,
																1);
}
void print_bytes_processed (result_bytes_processed * rbp,
														FILE * outstream);
inline bool is_result_good (result_bytes_processed * rbp){
	return rbp->result;
}

typedef struct{
	result_bytes_processed * bytes_read;
	result_bytes_processed * bytes_written;
} result_bytes_processed_pair;
void free_result_bytes_processed_pair (result_bytes_processed_pair rbpp);
// bool is_bytes_read_good (result_bytes_processed_pair rbpp);
// bool is_bytes_written_good (result_bytes_processed_pair rbpp);
// bool are_all_results_good (result_bytes_processed_pair rbpp);

typedef struct{
	result_bytes_processed_pair prev_file_processed;
	result_bytes_processed_pair cur_file_processed;
} compare_two_result_bytes_processed;
void free_compare_two_result_bytes_processed (compare_two_result_bytes_processed ctrbp);
// bool is_prev_file_result_good (compare_two_result_bytes_processed ctrbp);
// bool is_cur_file_result_good (compare_two_result_bytes_processed ctrbp);
// bool are_both_file_results_good (compare_two_result_bytes_processed ctrbp);

#endif /*___UTILITIES_H___*/
