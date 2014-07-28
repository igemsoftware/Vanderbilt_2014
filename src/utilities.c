#include "utilities.h"

extern inline string_with_size * make_new_string_with_size_ptr (size_t full_size);

extern inline void free_string_with_size_ptr(string_with_size * sws_to_free);

result_number_processed * initialize_result_number_processed_ptr(result_number_processed * rbp){
	rbp->result = 0;
	mpz_init(rbp->number_processed); // value set to 0
	return rbp;
}

void free_result_number_processed_ptr(result_number_processed * rbp){
	mpz_clear(rbp->number_processed);
	free(rbp);
}

extern inline result_number_processed * add_to_number_processed_ptr(result_number_processed * rbp, unsigned long int added_bytes);

extern inline result_number_processed * increment_number_processed_ptr(result_number_processed * rbp);

void print_number_processed_ptr(result_number_processed * rbp, FILE * outstream){
	mpz_out_str(outstream,
							10,								// base 10
							rbp->number_processed);
}

void free_result_number_processed_pair(result_number_processed_pair rbpp){
	free_result_number_processed_ptr(rbpp.bytes_read);
	free_result_number_processed_ptr(rbpp.bytes_written);
}
