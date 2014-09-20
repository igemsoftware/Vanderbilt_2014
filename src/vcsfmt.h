#ifndef ___VCS_FMT_H___
#define ___VCS_FMT_H___

/*
    functions for breaking down DNA sequences into specially formatted files
    which work well with traditional line-based version control software
*/

#include "block_processing.h" // for block processing methods

#define OUTPUT_SUFFIX ".vcsfmt"
#define META_SUFFIX	".vcsfmt.meta.tmp"

// TODO: javadoc
// if filetype supported, produces vcsfmt file of same name
void vcsfmt(char * filename);
void de_vcsfmt(char * filename); // produces original file

/**
 * Tranforms a block of fasta formatted ascii data into pure genetic data that can
 * be processed by the functions in block_processing.h.
 *
 * Basically removes new lines and meta-data. The meta-data for each block is separated out
 * into a separate structure that is annotated in such a way that the original fasta file can
 * be recovered by calling de_fasta_preformat. The meta-data returned from each call should be
 * written to a separate file, which can be appended to the end of the vcsfmt file.
 *
 * This function is capable of processing large files in chunks. Some parameters, like in_comment,
 * should be passed in to each sequential call so that the function remembers key information about
 * the last chunk it processed.
 *
 * @precondition	the size of output in memory is >= the size of input in memory.
 *
 * @param 	in_comment 	Whether or not we are currently in a fasta comment.
 */
string_with_size * fasta_preformat(string_with_size * input,
									string_with_size * output,
									string_with_size * metadata,
									bool * in_comment,
									int * lines_processed);

/**
 * A helper function for preformat that writes a line annotation to a string buffer.
 *
 * returns the number of characters written.
 */
int write_annotation(char * output, int line_number);

#endif /*___VCS_FMT_H___*/
