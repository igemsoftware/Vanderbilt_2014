#include <stdlib.h>							// for malloc
#include <string.h>							// for strlen/strcpy/strcat

#include "utilities.h"					// for compiler macros
#include "vcsfmt.h"							// required

int vcsfmt(char * filename, size_t block_size){
	FILE * input_file = open_file(filename);
	PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NULL(input_file,"Error in creating input file.\n");

	// create filename long enough to concatenate filename and suffix
	char * output_file_name = (char*) malloc((strlen(filename) +
																						strlen(OUTPUT_SUFFIX) +
																						1) * sizeof(char));
	// create output filename
	strcpy(output_file_name,filename);
	strcat(output_file_name,OUTPUT_SUFFIX);
	FILE * output_file = create_outfile(output_file_name);
	PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NULL(output_file,"Error in creating output file.\n");

	// allocate mem for block
	string_with_size * stream_block = (string_with_size *) malloc(sizeof(string_with_size));
	stream_block->string = malloc((block_size) * sizeof(char));
	stream_block->full_size = block_size * sizeof(char);
	stream_block->cur_size = 0;		// no data written yet
	PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NULL(stream_block->string,"Stream memory block not successfully allocated.\n");

	size_t cur_bytes_read = 0;
	size_t total_bytes_read = 0;
	size_t cur_bytes_written = 0;
	size_t total_bytes_written = 0;

	// perform block processing
	while (!feof(input_file) && !ferror(input_file) && !ferror(output_file)){
		// read in block
		read_block(input_file, stream_block);
		cur_bytes_read = stream_block->cur_size;
		total_bytes_read += cur_bytes_read;

#if defined DEBUG
		fprintf(stderr,"interim cur_size: ");
		fprintf(stderr,"%zu",stream_block->cur_size);
		fprintf(stderr,"\ninterim full_size: ");
		fprintf(stderr,"%zu",stream_block->cur_size);
		fprintf(stderr,"\n---------------\n");
#endif

		
		// process and write block
		append_to_outfile(output_file,process_block(stream_block));
		cur_bytes_written = stream_block->cur_size;
		total_bytes_written += cur_bytes_written;
	}

	// cleanup
	free(output_file_name);
	free(stream_block->string);
	free(stream_block);
	fclose(input_file);
	fclose(output_file);

	// error handling
	if (ferror(input_file) && !feof(input_file)){
		PRINT_ERROR("Error in reading from input file.\n");
		return -1;
	}
	else if (ferror(output_file) && !feof(input_file)){
		PRINT_ERROR("Error in writing to output file.\n");
		return -1;
	}
	else{													// if reached EOF successfully
		// diagnostics
		printf("%zu",total_bytes_read);
		printf(" bytes read.\n");
		printf("%zu",total_bytes_written);
		printf(" bytes written.\n");
		return 0;
	}
}

FILE * open_file(char * filename){
	FILE * input_file = fopen(filename,"r");
	// reject if incorrect filetype
	return input_file;
}

void read_block(FILE * input_file, string_with_size * input_str_with_size){
	input_str_with_size->cur_size = fread(input_str_with_size->string,sizeof(char),input_str_with_size->full_size,input_file);
#if defined DEBUG
	fprintf(stderr,"read cur_size: ");
	fprintf(stderr,"%zu",input_str_with_size->cur_size);
	fprintf(stderr,"\nread full_size: ");
	fprintf(stderr,"%zu",input_str_with_size->cur_size);
	fprintf(stderr,"\n---------------\n");
#endif
}

string_with_size * process_block(string_with_size * active_str_with_size){ // does nothing right now
#if defined DEBUG
	fprintf(stderr,"process cur_size: ");
	fprintf(stderr,"%zu",active_str_with_size->cur_size);
	fprintf(stderr,"\nprocess full_size: ");
	fprintf(stderr,"%zu",active_str_with_size->cur_size);
	fprintf(stderr,"\n---------------\n");
#endif
	return active_str_with_size;
}

FILE * create_outfile(char* filename){
	FILE * output_file = fopen(filename,"w");
	return output_file;
}

void append_to_outfile(FILE * output_file, string_with_size * output_str_with_size){
#if defined DEBUG
	fprintf(stderr,"write cur_size: ");
	fprintf(stderr,"%zu",output_str_with_size->cur_size);
	fprintf(stderr,"\nwrite full_size: ");
	fprintf(stderr,"%zu",output_str_with_size->cur_size);
	fprintf(stderr,"\n---------------\n");
#endif
	// note that this only writes .cur_size bytes to output!
	// this is so we can handle process_block returning smaller strings than it was given
	// this also seems like a fruitful source of bugs
	output_str_with_size->cur_size = fwrite(output_str_with_size->string,sizeof(char),output_str_with_size->cur_size,output_file);
}
