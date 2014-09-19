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
    string_with_size * input_block_with_size =
      make_new_string_with_size(BLOCK_SIZE);
#ifndef CONCURRENT
    string_with_size * output_block_with_size =
      make_new_string_with_size(BINBLOCK_SIZE);
#endif
    bool is_within_orf = false; // file begins outside of orf
    size_t cur_orf_pos = 0;
    char current_codon_frame[CODON_LENGTH] = {'\0'};

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
    GThread * write_block_thread =
      g_thread_new("write_block_thread",
                   (GThreadFunc) concurrent_write_block_vcsfmt,
                   &args_to_write_block);
    g_thread_join(write_block_thread); // implicitly frees thread
#else
    while (!feof(input_file) && !ferror(input_file) && !ferror(output_file)) {
        // Read a block
    	read_block(input_file, input_block_with_size);

        // Remove new lines and
        write_block(output_file,
                    process_block_vcsfmt(input_block_with_size,
                                         output_block_with_size,
                                         &is_within_orf,
                                         &cur_orf_pos,
                                         current_codon_frame,
                                         feof(input_file)));
    }
#endif
// cleanup allocated memory and open handles
#ifdef CONCURRENT
    // TODO: fix mutex and thread memory leaks
    g_async_queue_unref(active_queue);
    free(is_processing_complete);
    g_mutex_clear(&process_complete_mutex);
    g_thread_unref(read_and_process_block_thread); // write thread freed by join
#endif
    free(output_file_name);
    free_string_with_size(input_block_with_size);
#ifndef CONCURRENT
    free_string_with_size(output_block_with_size);
#endif
    // error handling
    if (ferror(input_file) && !feof(input_file)) {
        PRINT_ERROR_WITH_NEWLINE("Error in reading from input file.");
    } else if (ferror(output_file) && !feof(input_file)) {
        PRINT_ERROR_WITH_NEWLINE("Error in writing to output file.");
    } else if (feof(input_file)) {
        PRINT_ERROR_WITH_NEWLINE("vcsfmt completed successfully.");
    } else {
        PRINT_ERROR_WITH_NEWLINE("Unknown error in vcsfmt.");
    }
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
      make_new_string_with_size(BINBLOCK_SIZE);
    // TODO: make concurrent
    string_with_size * output_block_with_size =
      make_new_string_with_size(BINBLOCK_SIZE);

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
        PRINT_ERROR_WITH_NEWLINE("Error in reading from input file.");
    } else if (ferror(output_file) && !feof(input_file)) {
        PRINT_ERROR_WITH_NEWLINE("Error in writing to output file.");
    } else if (feof(input_file)) {
        PRINT_ERROR_WITH_NEWLINE("de_vcsfmt completed successfully");
    } else {
        PRINT_ERROR_WITH_NEWLINE("Unknown error in de_vcsfmt");
    }
    // close open handles
    fclose(input_file);
    fclose(output_file);
}

string_with_size * fasta_preformat(string_with_size * input,
									string_with_size * output,
									string_with_size * metadata,
									bool * in_comment,
									int * lines_processed) {
		// Set the readable bytes of output and metadata to 0
		output->readable_bytes = 0;
		metadata->readable_bytes = 0;

		// Loop through all the readable characters in input.
		for (int i = 0; i < input->readable_bytes; ++i) {

			char current = input->string[i];

			if (current == '\n') {
				// The character is a line break
				if (*in_comment) {
					// If we were in a comment, write the line break to metadata
					metadata->string[metadata->readable_bytes] = '\n';
					metadata->readable_bytes++;

					// We're not in a comment anymore.
					*in_comment = false;
				}

				// Update the lines processed.
				(*lines_processed)++;
			}
			else if (*in_comment) {
				// We're in a comment.

				// If a comment from a previous chunk flowed into this one, we need to reannotate that line.
				if (i == 0) {
					// Write the annotation to metadata.
					write_annotation(&(metadata->string[metadata->readable_bytes]), lines_processed);
				}

				// Write the char to metadata.
				metadata->string[metadata->readable_bytes] = current;
			}
			else if (current == ';' || current == '>') {
				// We've found a comment.

				// Annotate the line that we found this comment.
				write_annotation(&(metadata->string[metadata->readable_bytes]), lines_processed);

				// Flag that we're in a comment now.
				*in_comment = true;

				// Write the char to metadata.
				metadata->string[metadata->readable_bytes] = current;
			}
			else {
				// Any other characters we're assuming is genetic data.
				output->string[output->readable_bytes] = current;

				output->readable_bytes++;
			}

		}
}

/**
 * A helper function for preformat that writes a line annotation to a string buffer.
 *
 * returns the number of characters written.
 */
int write_annotation(char * output, int line_number) {
		output[0] = '@';

		int written = sprintf(output + 1, "%d", line_number);

		output[written + 1] = '@';

		return written + 2;
}
