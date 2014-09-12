#ifndef ___BLOCK_PROCESSING_H___
#define ___BLOCK_PROCESSING_H___

/*
    functions to process strings of dna data in sequential blocks
*/

#include "string_processing.h" // provide codon sequence data to files

// chosen so that maximum BINBLOCK_SIZE is 8192, a power of 2 (heuristic for
// fast file I/O)
#define BLOCK_SIZE 7928

// only look for orfs above 60 bases long (heuristic)
#define MIN_ORF_LENGTH 60
// TODO: convert this to program option at some point, not compile-time
// definition

// output block maximum size
// maximum possible size of output block, assuming every possible 60-char
// sequence is an orf (which won't happen)
// two newlines per orf in the worst possible case
#define BINBLOCK_SIZE (size_t)(BLOCK_SIZE * (1 + 2 / (double) MIN_ORF_LENGTH))

FILE * open_file_read(char * filename);
FILE * create_outfile(char * filename);

static inline string_with_size *
  read_block(FILE * input_file, string_with_size * input_string_with_size) {
    input_string_with_size->readable_bytes =
      fread(input_string_with_size->string,
            sizeof(char),
            input_string_with_size->size_in_memory,
            input_file);
    return input_string_with_size;
}

static inline string_with_size *
  write_block(FILE * output_file, string_with_size * output_block_with_size) {
    output_block_with_size->readable_bytes =
      fwrite(output_block_with_size->string,
             sizeof(char),
             output_block_with_size->readable_bytes,
             output_file);
    return output_block_with_size;
}

string_with_size * process_block(string_with_size * input_block_with_size,
                                 string_with_size * output_block_with_size,
                                 bool * is_within_orf,
                                 size_t * cur_orf_pos,
                                 char * current_codon_frame,
                                 bool is_final_block);

string_with_size * de_process_block(string_with_size * input_block_with_size,
                                    string_with_size * output_block_with_size);

#ifdef CONCURRENT
// send arguments to queue
typedef struct {
    FILE * input_file;
    string_with_size * input_block_with_size;
    string_with_size * output_block_with_size;
    bool * is_within_orf;
    size_t * cur_orf_pos;
    char * current_codon_frame;
    bool is_final_block;
    GAsyncQueue * active_queue;
    result_bytes_processed * total_bytes_read;
    volatile bool * is_processing_complete;
    GMutex * process_complete_mutex;
} concurrent_read_and_process_block_args_vcsfmt;
typedef struct {
    FILE * active_file;
    string_with_size * active_block_with_size;
    GAsyncQueue * active_queue;
    result_bytes_processed * total_bytes_written;
    volatile bool * is_processing_complete;
    GMutex * process_complete_mutex;
} concurrent_read_write_block_args;

void concurrent_read_and_process_block(
  concurrent_read_and_process_block_args_vcsfmt * args);

void concurrent_write_block_vcsfmt(concurrent_read_write_block_args * args);

static inline bool is_processing_complete_vcsfmt_concurrent(
  concurrent_read_write_block_args * args) {
    if (g_async_queue_length(args->active_queue) != 0) {
        return false;
    } else {
        // OPTIMIZATION: make this variable static somehow
        bool result;
        g_mutex_lock(args->process_complete_mutex);
        result = *args->is_processing_complete;
        g_mutex_unlock(args->process_complete_mutex);
        return result;
    }
}
#endif
#endif /*___BLOCK_PROCESSING_H___*/
