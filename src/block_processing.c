#include "block_processing.h"

char special_chars[NUM_SPECIAL_CHARS] = {'\r','\n'};
char special_char_escapes[NUM_SPECIAL_CHARS];
bool special_char_escapes_created = false;

extern inline int create_special_char_escapes();

extern inline char escape_special_chars(char input_byte);

extern inline char de_escape_special_chars(char input_byte);

extern inline void delimit_block_by_line(string_with_size * input_block_with_size, string_with_size * output_block_with_size, bool * is_within_orf, size_t * cur_orf_pos);

extern inline void unhash_and_remove_newlines(string_with_size * input_block_with_size, string_with_size * output_block_with_size);
