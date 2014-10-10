#ifndef ___BLOCK_PROCESSING_H___
#define ___BLOCK_PROCESSING_H___

/*
    functions to process strings of dna data in sequential blocks
*/

#include "string_processing.h" // provide codon sequence data to files

// chosen so that maximum BIN_BLOCK_SIZE is 8192, a power of 2 (heuristic for
// fast file I/O)
// TODO: rename this to something more indicative of usage
#define BLOCK_SIZE 7928

// only look for orfs above 60 bases long (heuristic)
#define MIN_ORF_LENGTH 60
// TODO: convert this to program option at some point, not compile-time
// definition

// output block maximum size
// maximum possible size of output block, assuming every possible 60-char
// sequence is an orf (which won't happen)
// two newlines per orf in the worst possible case
#define BIN_BLOCK_SIZE (size_t)(BLOCK_SIZE * (1 + 2 / (double) MIN_ORF_LENGTH))

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

static inline mpz_t * increment_mpz_t(mpz_t * in) {
    mpz_add_ui(*in, *in, 1);
    return in;
}

static inline bool less_than_mpz_t(mpz_t * lhs, mpz_t * rhs) {
    return mpz_cmp(*lhs, *rhs) < 0;
}

// CLOBBERS CUR_LINE BY SETTING EQUAL TO FINAL_LINE
// also doesn't do anything of final_line <= cur_line
static inline FILE * advance_file_to_line(FILE * file,
                                          mpz_t * cur_line,
                                          mpz_t * final_line,
                                          size_t block_size) {
    if (!less_than_mpz_t(cur_line, final_line)) {
#ifdef DEBUG
        PRINT_ERROR("NOT ADVANCING FILE!!!!!!!");
#endif
        return file;
    } else {
        string_with_size * in_block = make_new_string_with_size(block_size);
        bool succeeded = false;
        while (!succeeded && !(feof(file) || ferror(file))) {
            read_block(file, in_block);
            for (size_t block_index = 0; block_index < in_block->readable_bytes;
                 ++block_index) {
                if (NEWLINE == in_block->string[block_index]) {
                    increment_mpz_t(cur_line);
                    if (!less_than_mpz_t(cur_line, final_line)) { // if ==
                        // go back to beginning of line
                        // IFFY: casts here could potentially cause annoyance
                        // long used because fseek expects long
                        fseek(file,
                              ((long) block_index) -
                                ((long) in_block->readable_bytes - 1),
                              SEEK_CUR);
                        succeeded = true;
                        break;
                    }
                }
            }
        }
        free_string_with_size(in_block);
        mpz_set(*cur_line, *final_line);
        return file;
    }
}

static inline FILE *
  advance_file_one_line(FILE * file, mpz_t * cur_line, size_t block_size) {
    mpz_t final_line_one_up;
    mpz_init_set(final_line_one_up, *cur_line);
    increment_mpz_t(&final_line_one_up);
    FILE * advanced_file =
      advance_file_to_line(file, cur_line, &final_line_one_up, block_size);
    mpz_clear(final_line_one_up);
    return advanced_file;
}

// INCLUDES NEWLINE
static inline void write_current_line_of_file(FILE * source_file,
                                              FILE * dest_file) {
    string_with_size * io_block = make_new_string_with_size(BIN_BLOCK_SIZE);
    bool succeeded = false;
    mpz_t cur_point_in_line;
    mpz_init(cur_point_in_line); // set to 0
    while (!succeeded && !(feof(source_file) || ferror(source_file))) {
        read_block(source_file, io_block);
        for (size_t block_index = 0; block_index < io_block->readable_bytes;
             ++block_index) {
            if (NEWLINE == io_block->string[block_index]) {
                // go back to beginning of line
                // IFFY: casts here could potentially cause annoyance
                // long used because fseek expects long
                while (mpz_cmp_ui(cur_point_in_line, BIN_BLOCK_SIZE) >=
                       0) { // while chars in line > size of a
                            // single block
                    // seek back to beginning of line
                    fseek(source_file, -((long) BIN_BLOCK_SIZE), SEEK_CUR);
                    mpz_sub_ui(
                      cur_point_in_line, cur_point_in_line, BIN_BLOCK_SIZE);
                }
                fseek(source_file,
                      ((long) block_index) - ((long) io_block->readable_bytes),
                      SEEK_CUR);
                // +1 is to include the newline at the end
                set_string_with_size_readable_bytes(io_block, block_index + 1);
                succeeded = true;
                break;
            }
        }
        write_block(dest_file, io_block);
        mpz_add_ui(cur_point_in_line, cur_point_in_line, BIN_BLOCK_SIZE);
    }
    free_string_with_size(io_block);
    mpz_clear(cur_point_in_line);
}

// CLOBBERS FROM_LINE_NUMBER
// i.e. increment it
static inline void write_single_line_from_file_to_file(mpz_t * from_line_number,
                                                       FILE * source_file,
                                                       FILE * dest_file) {
    increment_mpz_t(from_line_number);
    write_current_line_of_file(source_file, dest_file);
}

// CLOBBERS FROM_LINE_NUMBER
// i.e. sets it equal to to_line_number
static inline void
  write_line_number_from_file_to_file(mpz_t * from_line_number,
                                      mpz_t * to_line_number,
                                      FILE * source_file,
                                      FILE * dest_file) {
    advance_file_to_line(
      source_file, from_line_number, to_line_number, BIN_BLOCK_SIZE);
    write_current_line_of_file(source_file, dest_file);
    mpz_set(*from_line_number, *to_line_number);
}

  /**
   *  Given a continuous stream of DNA characters, this function will insert
   *newline characters
   *  in between genes and junk DNA. In other words, each line in the output
   *will
   *be either a gene
   *  or junk DNA.
   *
   *  This function expects the input to only contain DNA characters (no new
   *lines
   *or anything else).
   *
   *  This function is also written to be able to process data in multiple
   *chunks.
   *If multiple calls are
   *  made to this function for different chunks of the same data, the same
   *parameters should be passed
   *  in each call. This will let the function remember key information about
   *the
   *last chunk it processed.
   */
  string_with_size
  * process_block_vcsfmt(string_with_size * input_block_with_size,
                         string_with_size * output_block_with_size,
                         bool * is_within_orf,
                         size_t * cur_orf_pos,
                         char * current_codon_frame,
                         bool is_final_block);

// TODO: javadoc this
string_with_size *
  de_process_block_vcsfmt(string_with_size * input_block_with_size,
                          string_with_size * output_block_with_size);

#ifdef CONCURRENT
// send arguments to queue
// used as variadic arguments to function
// concurrent_read_and_process_block_vcsfmt for GThread
// TODO: obv javadoc
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
// used as variadic arguments to function
// concurrent_read_and_process_block_vcsfmt for GThread
// TODO: obv javadoc
typedef struct {
    FILE * active_file;
    string_with_size * active_block_with_size;
    GAsyncQueue * active_queue;
    result_bytes_processed * total_bytes_written;
    volatile bool * is_processing_complete;
    GMutex * process_complete_mutex;
} concurrent_read_write_block_args_vcsfmt;

// TODO: javadoc
void concurrent_read_and_process_block_vcsfmt(
  concurrent_read_and_process_block_args_vcsfmt * args);

// TODO: javadoc
void
  concurrent_write_block_vcsfmt(concurrent_read_write_block_args_vcsfmt * args);

// TODO: javadoc
// cool mutex stuff
static inline bool is_processing_complete_vcsfmt_concurrent(
  concurrent_read_write_block_args_vcsfmt * args) {
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
