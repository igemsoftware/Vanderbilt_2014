
#include "block_processing.h" // for block processing methods

#define OUTPUT_SUFFIX ".vcsfmt"
#define META_SUFFIX ".vcsfmt.meta.tmp"

/* cldoc:begin-category(fasta) */

/* The strategy for converting a FASTA file to a VCSFMT file.
 * @filename the FASTA file to produce a VCSFMT file from.
 *
 */
void fasta_vcsfmt(char * filename);

/* The strategy for converting a VCSFMT file to a FASTA file.
 * @filename the VCSFMT file to use to recreate the FASTA file.
 *
 */
void fasta_de_vcsfmt(char * filename);

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
 * the original fasta file can be recovered by calling fasta_postformat. The
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
 * @metadata_byte_length the number of bytes of metadata at the end of the VCSFMT file
 *
 * Currently a FASTA header looks like this:
 *
 * VCSFMT;FASTA;<Line Alignment>;<Metadata byte count>
 *
 * Line alignment is the length of a line in the FASTA file. Genetic data in FASTA files are
 * snapped to a certain line length.
 *
 * Metadata byte count is the number of bytes of meta data pulled out of the FASTA file.
 *
 * Writes a header line to a VCSFMT file, with information needed to reconstruct
 * the FASTA file exactly from the VCSFMT file.
 */
void fasta_write_header(FILE * vcsfmt_file,
                        FILE * fasta_file,
                        long metadata_byte_length);

/*
 * Parse a fasta header to get relevant information.
 * @header			the header to be parsed
 * @line_alignment  a pointer to the long that will hold the length of a line in the fasta file.
 * @metadata_size	a pointer to the long that will hold the number of bytes of the metadata.
 */
void fasta_parse_header(char * header, long * line_alignment, long * metadata_size);

/* A helper function for preformat that writes a line annotation to a string.
 * @output the string buffer to write to
 * @line_number the current index of the line in file
 *
 * Currently a line annotation looks like this:
 *
 * @<Line Number>@<Comment Data>
 *
 * Line Number is where the line number of the comment in the original file, and
 * Comment Data is the original comment.
 *
 * @return the number of characters written.
 */
int write_annotation(char * output, int line_number);

/* cldoc:end-category() */
