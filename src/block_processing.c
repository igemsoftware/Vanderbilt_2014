#include "block_processing.h"		// required

#define MIN_ORF_LENGTH 60				// convert this to program option at some point

void hash_and_delimit_block_by_line(string_with_size * input_block_with_size, string_with_size * output_block_with_size, bool * is_within_orf, size_t * cur_orf_pos){
	// OPTIMIZATION: make this static
	char current_codon[CODON_LENGTH];
  char output_byte;							// output of hash
	size_t bytes_written = 0;

	for (size_t codon_index = 0; codon_index < input_block_with_size->cur_size; codon_index += CODON_LENGTH){
		// copy over bases
		for (size_t base_index = 0; base_index < CODON_LENGTH; ++base_index){
			current_codon[base_index] = toupper(input_block_with_size->string[codon_index + base_index]); // toupper to get rid of the lowercase dna which sometimes appears
		}

		// hash input codon to make byte
		output_byte = escape_special_chars(get_byte_from_codon(current_codon)); // '\0' if not a real codon
#if defined DEBUG
		fwrite(current_codon,sizeof(char),CODON_LENGTH,stdout);
		printf(" : ");
		printf("%c\n",output_byte);
#endif
		
		// check if well-formed ('\0' means codon not found)
		if (output_byte != '\0'){		// branch predictions should be good on this one
			if (*is_within_orf){			// same here
				// OPTIMIZATION: could hash start/stop codons as well
			  if (*cur_orf_pos >= MIN_ORF_LENGTH && is_stop_codon(current_codon)){
					output_block_with_size->string[bytes_written] = output_byte;
					++bytes_written;
					output_block_with_size->string[bytes_written] = '\n'; // newline delimiting
					*is_within_orf = false;																// NOTE THE DIFFERENCE HERE FROM BELOW
					*cur_orf_pos = 0;
				}
				else{
					output_block_with_size->string[bytes_written] = output_byte;
					++*cur_orf_pos;
				}
			}
			else{
				if (is_start_codon(current_codon)){
					output_block_with_size->string[bytes_written] = output_byte;
					++bytes_written;
					output_block_with_size->string[bytes_written] = '\n'; // newline delimiting
					*is_within_orf = true;																// NOTE THE DIFFERENCE HERE FROM ABOVE
				}
				else{
					output_block_with_size->string[bytes_written] = output_byte;
				}
			}
			++bytes_written;
		}
		else{		// if non-codon found, assume end of dna encoding and exit
			output_block_with_size->cur_size = bytes_written;
			printf("%s\n","end of dna found. exiting.");
		}
	
	}

	output_block_with_size->cur_size = bytes_written;

#if defined DEBUG
	// input block
	fprintf(stderr,"block IN cur_size: ");
	fprintf(stderr,"%zu",input_block_with_size->cur_size);
	fprintf(stderr,"\nblock IN full_size: ");
	fprintf(stderr,"%zu\n",input_block_with_size->full_size);
	// output block
	fprintf(stderr,"block OUT cur_size: ");
	fprintf(stderr,"%zu",output_block_with_size->cur_size);
	fprintf(stderr,"\nblock OUT full_size: ");
	fprintf(stderr,"%zu",output_block_with_size->full_size);
	fprintf(stderr,"\n---------------\n");
#endif
}

void unhash_and_remove_newlines(string_with_size * input_block_with_size, string_with_size * output_block_with_size){
	// OPTIMIZATION: make this static
  char current_byte;
	const char * output_codon;
	size_t bytes_read = 0;
	size_t bytes_written = 0;

	while (bytes_read < input_block_with_size->cur_size){
		current_byte = input_block_with_size->string[bytes_read];
		if (current_byte != '\n'){
			// de-hash
			output_codon = get_codon_from_byte(de_escape_special_chars(current_byte));
			if (strcmp(output_codon,"") == 0){
				printf("BLANK\n");
			}
#if defined DEBUG
			printf("%c",current_byte);
			printf(" : ");
			for (size_t i = 0; i < CODON_LENGTH; ++i){
				// fwrite(output_codon,sizeof(char),CODON_LENGTH,stdout);
				printf("%c",output_codon[i]);
			}
			printf("\n%zu\n",bytes_written);
#endif
			for (size_t base_index = 0; base_index < CODON_LENGTH; ++base_index){
				output_block_with_size->string[bytes_written + base_index] = output_codon[base_index];
			}
			bytes_written += CODON_LENGTH;
		}
		++bytes_read;
	}
	output_block_with_size->cur_size = bytes_written;

#if defined DEBUG
	fprintf(stderr,"block cur_size: ");
	fprintf(stderr,"%zu",output_block_with_size->cur_size);
	fprintf(stderr,"\nblock full_size: ");
	fprintf(stderr,"%zu",output_block_with_size->full_size);
	fprintf(stderr,"\n---------------\n");
#endif
}


// CHARACTER ESCAPING
#define NUM_SPECIAL_CHARS 2
char special_chars[NUM_SPECIAL_CHARS] = {'\r','\n'}; // do not add '\0' to this list, it is used to indicate a byte was not found in get_byte_from_codon

char special_char_escapes[NUM_SPECIAL_CHARS];
 
int create_special_char_escapes(){
	bool found_space = true;
	for (size_t special_char_index = 0; found_space && special_char_index < NUM_SPECIAL_CHARS; ++special_char_index){
		// start from previously found highest index of hash array which was NOT found to contain strings in the hash (is an empty string "")
		// iterate through indices until finding next unoccupied space in hash array
		// if go all the way through and do not find one, exit
		found_space = false;
		for (size_t wordlist_index = special_char_index; !found_space && wordlist_index < MAX_HASH_VALUE; ++wordlist_index){
			if (wordlist_index == 0){	// diff tools often check if there are any null bytes in the first sev eral thousand bytes of a file in order to determine whether it is binary
				++wordlist_index;				// this means no escape character gets sent to 0x00, and avoids that
			}
			if (strcmp(wordlist[wordlist_index], "") == 0){
				found_space = true;
				special_char_escapes[special_char_index] = (char) wordlist_index;
			}
		}
	}
	if (found_space){							// if found space for all special_chars
		return 0;										// success
	}
	else{
		return -1;									// failure
	}
}

char escape_special_chars(char input_byte){
	char return_byte = input_byte;
	for (size_t escape_char_index = 0; escape_char_index < NUM_SPECIAL_CHARS; ++escape_char_index){
		if (input_byte == special_chars[escape_char_index]){
			return_byte = special_char_escapes[escape_char_index];
		}
	}
	return return_byte;
}

char de_escape_special_chars(char input_byte){
	char return_byte = input_byte;
	for (size_t de_escape_char_index = 0; de_escape_char_index < NUM_SPECIAL_CHARS; ++de_escape_char_index){
		if (input_byte == special_char_escapes[de_escape_char_index]){
			return_byte = special_chars[de_escape_char_index];
		}
	}
	return return_byte;
}