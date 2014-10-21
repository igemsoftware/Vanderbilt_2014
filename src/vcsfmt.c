#include "vcsfmt.h" // required

// VCSFMT
void vcsfmt(char * filename) {
    FILE * input_file = open_file_read(filename);
    PRINT_ERROR_AND_RETURN_IF_NULL(input_file, "Error in creating input file.");

    // create output filename
    char * output_file_name =
      malloc((strlen(filename) + strlen(OUTPUT_SUFFIX) + 1) * sizeof(char));
    strcpy(output_file_name, filename);
    strcat(output_file_name, OUTPUT_SUFFIX);
    FILE * output_file = create_file_binary_write(output_file_name);
    PRINT_ERROR_AND_RETURN_IF_NULL(output_file,
                                   "Error in creating output file.");

    // create temporary file for metadata
    char * temporary_file_name =
      malloc((strlen(filename) + strlen(META_SUFFIX) + 1) * sizeof(char));
    strcpy(temporary_file_name, filename);
    strcat(temporary_file_name, META_SUFFIX);
    FILE * temporary_file = create_file_binary_write(temporary_file_name);
    PRINT_ERROR_AND_RETURN_IF_NULL(temporary_file,
                                   "Error in creating temp file for meta data.")

    string_with_size * input_block_with_size =
      make_new_string_with_size(BLOCK_SIZE);
#ifndef CONCURRENT
    string_with_size * output_block_with_size =
      make_new_string_with_size(BIN_BLOCK_SIZE);

    // These constructs are used by the FASTA preformating function.
    // TODO put some checks for file type here.
    string_with_size * preformatted_block_with_size =
      make_new_string_with_size(BLOCK_SIZE);

    string_with_size * metadata_block_with_size =
      make_new_string_with_size(BIN_BLOCK_SIZE);

    bool in_comment = false; // file beings outside a comment.

    int lines_pre_processed = 0; // We haven't preprocessed any lines yet.

#endif
    bool is_within_orf = false; // file begins outside of orf
    unsigned long long cur_orf_pos = 0;
    char current_codon_frame[ CODON_LENGTH ] = {'\0'};

#ifdef CONCURRENT
    GAsyncQueue * active_queue = g_async_queue_new();
    bool * is_processing_complete = malloc(sizeof(bool));
    *is_processing_complete = false;
    GMutex process_complete_mutex;
    g_mutex_init(&process_complete_mutex);

    concurrent_read_and_process_block_args_vcsfmt args_to_block_processing;
    args_to_block_processing.input_file = input_file;
    args_to_block_processing.input_block_with_size = input_block_with_size;
    args_to_block_processing.is_within_orf = &is_within_orf;
    args_to_block_processing.cur_orf_pos = &cur_orf_pos;
    args_to_block_processing.current_codon_frame = current_codon_frame;
    args_to_block_processing.is_final_block = false;
    args_to_block_processing.active_queue = active_queue;
    args_to_block_processing.total_bytes_read = total_bytes_read;
    args_to_block_processing.is_processing_complete = is_processing_complete;
    args_to_block_processing.process_complete_mutex = &process_complete_mutex;

    concurrent_read_write_block_args_vcsfmt args_to_write_block;
    args_to_write_block.active_file = output_file;
    args_to_write_block.active_queue = active_queue;
    args_to_write_block.total_bytes_written = total_bytes_written;
    args_to_write_block.is_processing_complete = is_processing_complete;
    args_to_write_block.process_complete_mutex = &process_complete_mutex;

    GThread * read_and_process_block_thread =
      g_thread_new("read_and_process_block_thread",
                   (GThreadFunc) concurrent_read_and_process_block,
                   &args_to_block_processing);
    GThread * write_block_thread = g_thread_new(
      "write_block_thread", (GThreadFunc) concurrent_write_block_vcsfmt,
      &args_to_write_block);
    g_thread_join(write_block_thread); // implicitly frees thread
#else

    // Allocate a 60 char line for the VCSFMT header.
    for (int i = 0; i < 60; ++i)
        putc(' ', output_file);

    putc(NEWLINE, output_file);

    // keep track of how much metadata we have.
    long metadata_bytes = 0;

    while (!feof(input_file) && !ferror(input_file) && !ferror(output_file) &&
           !ferror(temporary_file)) {
        // Read a block
        read_block(input_file, input_block_with_size);

        // Preprocess the block from FASTA (i.e. remove newlines and metadata).
        // TODO - put some file type checks here. (We don't always use FASTA)
        fasta_preformat(input_block_with_size, preformatted_block_with_size,
                        metadata_block_with_size, &in_comment,
                        &lines_pre_processed);

        // Process the block and write it to output.
        write_block(output_file,
                    process_block_vcsfmt(preformatted_block_with_size,
                                         output_block_with_size, &is_within_orf,
                                         &cur_orf_pos, current_codon_frame,
                                         feof(input_file)));

        // Write the metadata to a temporary file.
        write_block(temporary_file, metadata_block_with_size);

        // Update the number of metadata bytes we've written
        metadata_bytes += (long int) metadata_block_with_size->readable_bytes;
    }

    // error handling
    if (ferror(input_file) && !feof(input_file)) {
        PRINT_ERROR("Error in reading from input file.");
    } else if (ferror(output_file) && !feof(input_file)) {
        PRINT_ERROR("Error in writing to output file.");
    }

    // Add an extra newline to output
    fputc(NEWLINE, output_file);

    // Append the metadata file to the output file.
    fclose(temporary_file);
    temporary_file = fopen(temporary_file_name, "r");

    char c;
    while ((c = (char) fgetc(temporary_file)) != EOF) {
        fputc(c, output_file);
    }

    // Delete the temp file
    if (remove(temporary_file_name) != 0) {
        PRINT_ERROR("Could not delete metadata tmp file.");
    }

    // Write the fasta-specific header
    // TODO - put a file type check here (we're not always using fasta)
    fasta_write_header(output_file, input_file, metadata_bytes);

#endif
// cleanup allocated memory and open handles
#ifdef CONCURRENT
    // TODO: fix mutex and thread memory leaks (if they actually exist???)
    g_async_queue_unref(active_queue);
    free(is_processing_complete);
    g_mutex_clear(&process_complete_mutex);
    g_thread_unref(read_and_process_block_thread); // write thread freed by join
#endif
    free(output_file_name);
    free_string_with_size(input_block_with_size);
#ifndef CONCURRENT
    free_string_with_size(output_block_with_size);
    free_string_with_size(preformatted_block_with_size);
    free_string_with_size(metadata_block_with_size);
#endif
    // close open handles
    fclose(input_file);
    fclose(output_file);
}

// DE_VCSFMT
void de_vcsfmt(char * filename) {
    FILE * input_file = open_file_read(filename);
    PRINT_ERROR_AND_RETURN_IF_NULL(input_file, "Error in creating input file.");
    // create filename long enough to concatenate filename and suffix
    char * output_file_name =
      malloc((strlen(filename) + strlen(OUTPUT_SUFFIX) + 1) * sizeof(char));
    strcpy(output_file_name, filename);
    strcat(output_file_name, OUTPUT_SUFFIX);
    FILE * output_file = create_file_binary_write(output_file_name);
    PRINT_ERROR_AND_RETURN_IF_NULL(output_file,
                                   "Error in creating output file.");
    string_with_size * input_block_with_size =
      make_new_string_with_size(BIN_BLOCK_SIZE);
    // TODO: make concurrent
    string_with_size * output_block_with_size =
      make_new_string_with_size(BIN_BLOCK_SIZE);

    while (!feof(input_file) && !ferror(input_file) && !ferror(output_file)) {
        read_block(input_file, input_block_with_size);
        write_block(output_file,
                    de_process_block_vcsfmt(input_block_with_size,
                                            output_block_with_size));
    }

    // cleanup mem
    free(output_file_name);
    free_string_with_size(input_block_with_size);
    free_string_with_size(output_block_with_size);
    if (ferror(input_file) && !feof(input_file)) {
        PRINT_ERROR("Error in reading from input file.");
    } else if (ferror(output_file) && !feof(input_file)) {
        PRINT_ERROR("Error in writing to output file.");
    } else if (feof(input_file)) {
        PRINT_ERROR("de_vcsfmt completed successfully");
    } else {
        PRINT_ERROR("Unknown error in de_vcsfmt");
    }
    // close open handles
    fclose(input_file);
    fclose(output_file);
}

string_with_size * fasta_preformat(string_with_size * input,
                                   string_with_size * preformatted,
                                   string_with_size * metadata,
                                   bool * in_comment,
                                   int * lines_processed) {
    // Set the readable bytes of output and metadata to 0
    preformatted->readable_bytes = 0;
    metadata->readable_bytes = 0;

    // Loop through all the readable characters in input.
    for (unsigned long long i = 0; i < input->readable_bytes; ++i) {

        char current = input->string[ i ];

        if (current == NEWLINE) {
            // The character is a line break
            if (*in_comment) {
                // If we were in a comment, write the line break to metadata
                metadata->string[ metadata->readable_bytes ] = NEWLINE;
                metadata->readable_bytes++;

                // We're not in a comment anymore.
                *in_comment = false;
            }

            // Update the lines processed.
            (*lines_processed)++;
        } else if (*in_comment) {
            // We're in a comment.

            // Write the char to metadata.
            metadata->string[ metadata->readable_bytes ] = current;
            metadata->readable_bytes++;
        } else if (current == ';' || current == '>') {
            // We've found a comment.

            // Annotate the line that we found this comment.
            int written =
              write_annotation(&(metadata->string[ metadata->readable_bytes ]),
                               *lines_processed);

            metadata->readable_bytes += (unsigned long long) written;

            // Flag that we're in a comment now.
            *in_comment = true;

            // Write the char to metadata.
            metadata->string[ metadata->readable_bytes ] = current;
            metadata->readable_bytes++;
        } else {
            // Any other characters we're assuming is genetic data.
            preformatted->string[ preformatted->readable_bytes ] = current;

            preformatted->readable_bytes++;
        }
    }

    return preformatted;
}

void fasta_write_header(FILE * vcsfmt_file,
                        FILE * fasta_file,
                        long metadata_byte_length) {

    fseek(vcsfmt_file, 0, SEEK_SET);

    fprintf(vcsfmt_file, "VCSFMT;");
    fprintf(vcsfmt_file, "FASTA;");

    // Figure out the line length of the fasta file, to be included in the
    // header.
    fseek(fasta_file, 0, SEEK_SET);

    // Find the first line that's not a comment
    char * line = malloc(200 * sizeof(char));
    while (!feof(fasta_file) && !ferror(fasta_file)) {
        fgets(line, 199, fasta_file);
        if (line[ 0 ] != ';' && line[ 0 ] != '>' && strlen(line) > 0) {
            break;
        }
    }

    // Print the line length to the header (minus the newline)
    fprintf(vcsfmt_file, "%zd;", strlen(line) - 1);
    free(line);

    // Print the size of the metadata in bytes to the header
    fprintf(vcsfmt_file, "%ld", metadata_byte_length);
}

/**
 * A helper function for preformat that writes a line annotation to a string
 *buffer.
 *
 * returns the number of characters written.
 */
int write_annotation(char * output, int line_number) {
    output[ 0 ] = '@';

    int written = sprintf(output + 1, "%d", line_number);

    output[ written + 1 ] = '@';

    return written + 2;
}
