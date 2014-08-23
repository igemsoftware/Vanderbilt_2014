#include "vcscmp.h" 						// required
#include "block_processing.h"		// for read_block and preprocessor defines

#define QUEUE_HASH_CRITICAL_SIZE 5 // largest size it can reach before it begins to be cleared out
// alternately, the largest difference between lines...???
// TODO: FIGURE THIS OUT

compare_two_result_bytes_processed vcscmp (char * prev_filename,
																					 char * cur_filename){
	// produce two queues of hashes
	// determine which lines are changes of previous lines
	// write out lines to file (from left or right as appropriate)
	// add changes at bottom (as appropriate)
	compare_two_result_bytes_processed results_for_both_files;
	// TODO: initialize the result values
	pre_format_file_vcscmp(prev_filename);
	FILE * prev_file = open_file_read(prev_filename);
	PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NULL(prev_file,
																									"Error in reading prev file.",
																									, // no expr needed
																									results_for_both_files);
	pre_format_file_vcscmp(cur_filename);
	FILE * cur_file = open_file_read(cur_filename);
	PRINT_ERROR_AND_PERFORM_EXPR_AND_RETURN_IF_NULL(cur_file,
																									"Error in reading cur file.",
																									, // no expr needed
																									results_for_both_files);
	
#ifdef CONCURRENT
#else
	GQueue * prev_file_hashes_queue = g_queue_new();
	GQueue * cur_file_hashes_queue = g_queue_new();

	while (!feof(prev_file) &&
				 !ferror(prev_file) &&
				 !feof(cur_file) &&
				 !ferror(cur_file)){
		while (g_queue_get_length(prev_file_hashes_queue) < QUEUE_HASH_CRITICAL_SIZE){
			// fill with stuff
		}
		while (g_queue_get_length(cur_file_hashes_queue) < QUEUE_HASH_CRITICAL_SIZE){
			// fill with stuff
		}
		// clear out queues
	}

	// free memory and close open handles
	g_queue_free(prev_file_hashes_queue);
	g_queue_free(cur_file_hashes_queue);
#endif
	return results_for_both_files;
}
