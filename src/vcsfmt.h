#ifndef ___VCS_FMT_H___
#define ___VCS_FMT_H___

/*
    functions for breaking down DNA sequences into specially formatted files
    which work well with traditional line-based version control software
*/

#include "block_processing.h" // for block processing methods

#define OUTPUT_SUFFIX ".vcsfmt"
#define META_SUFFIX ".vcsfmt.meta.tmp"

/* cldoc:begin-category(vcsfmt) */

/* Produces formatted file with extension ".vcsfmt".
 * @filename the file to produce a vcsfmt file from.
 *
 * vcsfmt file produced in same directory as given file.
 */
void vcsfmt(char * filename);
/* Produces unformatted file from vcsfmt file.
 * @filename the file to produce an unformatted version of.
 *
 * Unformatted file produced in same directory as vcsfmt file.
 */
void de_vcsfmt(char * filename);

/* cldoc:end-category() */

/* cldoc:begin-category(fasta_preformat) */

/* Tranforms block of fasta data into pure genetic data.
 * @input the input string of fasta data
 * @output a raw string of memory to write unformatted genetic data into
 * @metadata a string giving additional information about the fasta data
 * @in_comment whether or not we are currently in a fasta comment.
 * @lines_processed the number of lines previously processed
 *
 * Tranforms a block of fasta formatted ascii data into pure genetic data that
 * can be processed by the functions in block_processing.h.
 *
 * Basically removes new lines and meta-data. The meta-data for each block is
 * separated out into a separate structure that is annotated in such a way that
 * the original fasta file can be recovered by calling de_fasta_preformat. The
 * meta-data returned from each call should be written to a separate file, which
 * can be appended to the end of the vcsfmt file.
 *
 * This function is capable of processing large files in chunks. Some
 * parameters, like in_comment, should be passed in to each sequential call so
 * that the function remembers key information about the last chunk it
 * processed.
 *
 * Precondition: size of output in memory >= the size of input in memory.
 *
 * @return a raw block of genetic data
 */
string_with_size * fasta_preformat(string_with_size * input,
                                   string_with_size * output,
                                   string_with_size * metadata,
                                   bool * in_comment,
                                   int * lines_processed);

/* Write header to vcsfmt file.
 * @vcsfmt_file the formatted file to write to
 * @fasta_file the input file to read header data from
 * @metadata_byte_length the length of data in the header
 *
 * Writes a header line to a VCSFMT file, with information needed to reconstruct
 * the FASTA file exactly from the VCSFMT file.
 */
void fasta_write_header(FILE * vcsfmt_file,
                        FILE * fasta_file,
                        long metadata_byte_length);

/* A helper function for preformat that writes a line annotation to a string.
 * @output the string buffer to write to
 * @line_number the current index of the line in file
 *
 * @return the number of characters written.
 */
int write_annotation(char * output, int line_number);

/* cldoc:end-category() */

#endif /*___VCS_FMT_H___*/
