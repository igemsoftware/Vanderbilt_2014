#include "utilities.h"					// required

FILE * open_file_read (char * filename){
	FILE * input_file = fopen(filename,"r");
	return input_file;
}

FILE * create_file_binary_write (char * filename){
	FILE * output_file = fopen(filename,"wb");
	return output_file;
}

extern inline string_with_size * make_new_string_with_size (size_t size_in_memory);

extern inline void free_string_with_size (string_with_size * sws_to_free);

result_bytes_processed * initialize_result_bytes_processed (result_bytes_processed * rbp){
	rbp->result = 0;
	mpz_init(rbp->bytes_processed); // value set to 0
	return rbp;
}

void free_result_bytes_processed (result_bytes_processed * rbp){
	mpz_clear(rbp->bytes_processed);
	free(rbp);
}

extern inline result_bytes_processed * add_to_bytes_processed (result_bytes_processed * rbp,
																															 unsigned long int added_bytes);

extern inline result_bytes_processed * increment_bytes_processed (result_bytes_processed * rbp);

extern inline result_bytes_processed * add_to_lines_processed (result_bytes_processed * rbp,
																															 unsigned long int added_lines);

extern inline result_bytes_processed * increment_lines_processed (result_bytes_processed * rbp);

void print_bytes_processed (result_bytes_processed * rbp,
																FILE * outstream){
	mpz_out_str(outstream,
							10,								// base 10
							rbp->bytes_processed);
}

extern inline bool is_result_good (result_bytes_processed * rbp);

void free_result_bytes_processed_pair (result_bytes_processed_pair rbpp){
	free_result_bytes_processed(rbpp.bytes_read);
	free_result_bytes_processed(rbpp.bytes_written);
}

// bool is_bytes_read_good (result_bytes_processed_pair rbpp){
// 	return is_result_good(rbpp.bytes_read);
// }

// bool is_bytes_written_good (result_bytes_processed_pair rbpp){
// 	return is_result_good(rbpp.bytes_written);
// }

// bool are_all_results_good (result_bytes_processed_pair rbpp){
// 	return is_bytes_read_good(rbpp) && is_bytes_written_good(rbpp);
// }

void free_compare_two_result_bytes_processed (compare_two_result_bytes_processed ctrbp){
	free_result_bytes_processed_pair(ctrbp.prev_file_processed);
	free_result_bytes_processed_pair(ctrbp.cur_file_processed);
}

// bool is_prev_file_result_good (compare_two_result_bytes_processed ctrbp){
// 	return are_all_results_good(ctrbp.prev_file_processed);
// }

// bool is_cur_file_result_good (compare_two_result_bytes_processed ctrbp){
// 	return are_all_results_good(ctrbp.cur_file_processed);
// }

// bool are_both_file_results_good (compare_two_result_bytes_processed ctrbp){
// 	return is_prev_file_result_good(ctrbp) && is_cur_file_result_good(ctrbp);
// }
