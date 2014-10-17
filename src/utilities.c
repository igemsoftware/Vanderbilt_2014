#include "utilities.h"

FILE * open_file_read(const char * filename) {
    FILE * input_file = fopen(filename, "r");
    return input_file;
}
FILE * create_file_binary_write(const char * filename) {
    FILE * output_file = fopen(filename, "wb");
    return output_file;
}

// TODO: javadoc
// note that sets readable_bytes to 0
string_with_size *
  make_new_string_with_size(unsigned long long size_in_memory) {
    string_with_size * sws_to_return = malloc(sizeof(string_with_size));
    sws_to_return->string = malloc(size_in_memory * (sizeof(char)));
    sws_to_return->readable_bytes = 0;
    sws_to_return->size_in_memory = size_in_memory;
    return sws_to_return;
}
#ifdef DEBUG
// DOES NOT COPY OVER NULL CHAR TERMINATING
string_with_size *
  make_new_string_with_size_given_string(char * null_term_str) {
    string_with_size * sws_to_return = malloc(sizeof(string_with_size));
    sws_to_return->string = malloc(sizeof(char) * (strlen(null_term_str)));
    memcpy(sws_to_return->string, null_term_str, strlen(null_term_str));
    sws_to_return->readable_bytes = strlen(null_term_str);
    return sws_to_return;
}
#endif
// TODO: javadoc
string_with_size *
  set_string_with_size_readable_bytes(string_with_size * sws,
                                      unsigned long long readable_bytes) {
    sws->readable_bytes = readable_bytes;
    return sws;
}
// assumes enough memory in to_sws exists to handle this operation
string_with_size * copy_string_with_size(string_with_size * from_sws,
                                         string_with_size * to_sws) {
    for (unsigned long long index = 0; index < from_sws->readable_bytes;
         ++index) {
        to_sws->string[index] = from_sws->string[index];
    }
    return to_sws;
}
string_with_size * grow_string_with_size(string_with_size ** sws,
                                         unsigned long long final_size_in_mem) {
    if (final_size_in_mem > (*sws)->size_in_memory) {
        // double size of sws for amortized constant time growth
        // +1 catches size == 0
        unsigned long long optimal_size = (*sws)->size_in_memory + 1;
        while (optimal_size < final_size_in_mem) {
            optimal_size *= 2;
        }
        string_with_size * new_sws = make_new_string_with_size(optimal_size);
        new_sws->readable_bytes = (*sws)->readable_bytes;
        copy_string_with_size(*sws, new_sws);
        free_string_with_size(*sws);
        *sws = new_sws; // guaranteed not to fail
    }
    return *sws;
}
// TODO: javadoc
void free_string_with_size(void * arg) {
    if (NULL != arg) {
        string_with_size * sws_to_free = (string_with_size *)arg;
        free(sws_to_free->string);
        free(sws_to_free);
    }
}
