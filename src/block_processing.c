#include "block_processing.h"


extern inline string_with_size * delimit_block_by_line (string_with_size * input_block_with_size_,
																												string_with_size * output_block_with_size,
																												bool * is_within_orf,
																												size_t * cur_orf_pos,
																												char * current_codon_frame,
																												bool is_final_block);

extern inline string_with_size * remove_newlines (string_with_size * input_block_with_size,
																									string_with_size * output_block_with_size);

extern inline string_with_size * read_block (FILE * input_file,
																						 string_with_size * input_string_with_size);

extern inline string_with_size * process_block_vcsfmt (string_with_size * input_block_with_size,
																											 string_with_size * output_block_with_size,
																											 bool * is_within_orf,
																											 size_t * cur_orf_pos,
																											 char * current_codon_frame,
																											 bool is_final_block);

extern inline string_with_size * de_process_block_vcsfmt (string_with_size * input_block_with_size,
																													string_with_size * output_block_with_size);

extern inline string_with_size * write_block (FILE * output_file,
																							string_with_size * output_block_with_size);
#ifdef CONCURRENT
void read_and_process_block_vcsfmt_CONCURRENT (read_and_process_block_args_vcsfmt_CONCURRENT * args){
	while (!feof(args->input_file) && !ferror(args->input_file)){
		add_to_bytes_processed(args->total_bytes_read,
													 read_block(args->input_file,
																			args->input_block_with_size)->readable_bytes);
		// OPTIMIZATION: allocate from (possibly self-growing) pool of memory instead of constantly heap allocating a completely new block
		// if doing so, use gasyncqueue only to pass indices into this pool, instead of the actual memory blocks themselves
		args->output_block_with_size = make_new_string_with_size(BINBLOCK_SIZE);
		process_block_vcsfmt(args->input_block_with_size,
												 args->output_block_with_size,
												 args->is_within_orf,
												 args->cur_orf_pos,
												 args->current_codon_frame,
												 feof(args->input_file));
		if (feof(args->input_file)){ // if last loop
			g_mutex_lock(args->process_complete_mutex);
		}
		g_async_queue_push(args->active_queue,args->output_block_with_size);
	}
	*args->is_processing_complete = true;
	g_mutex_unlock(args->process_complete_mutex);
}

void write_block_vcsfmt_CONCURRENT (read_write_block_args_CONCURRENT * args){
	while (!is_processing_complete_vcsfmt_CONCURRENT(args)){
		g_mutex_unlock(args->process_complete_mutex);
		args->active_block_with_size = (string_with_size *) g_async_queue_pop(args->active_queue);
		add_to_bytes_processed(args->total_bytes_written,
													 write_block(args->active_file,
																			 args->active_block_with_size)->readable_bytes);
		free_string_with_size(args->active_block_with_size);
	}
	// get the rest left over (if any)
	for (size_t queue_size = g_async_queue_length(args->active_queue);
			 queue_size != 0;
			 --queue_size){
		args->active_block_with_size = (string_with_size *) g_async_queue_pop(args->active_queue);
		add_to_bytes_processed(args->total_bytes_written,
													 write_block(args->active_file,
																			 args->active_block_with_size)->readable_bytes);
		free_string_with_size(args->active_block_with_size);
	}
}

extern inline bool is_processing_complete_vcsfmt_CONCURRENT (read_write_block_args_CONCURRENT * args);
#endif

extern inline unsigned long djb2_hash_on_string_index (unsigned long instantaneous_hash,
																											 char * str,
																											 size_t cur_index);
