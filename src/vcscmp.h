#ifndef ___VCS_CMP_H___
#define ___VCS_CMP_H___

/*
    functions and utilities for implementing character-by-character
    comparison on .vcsfmt-formatted files
*/

// this file is where the money is

#include "string_processing.h"

// go through both files, producing list of hashes for each line (again, block
// I/O)

/*
essentially:
* want the COMPLETE lack of collisions that a perfect hash function has
    * this is critical to determining whether a line is inserted/deleted
* without the (linear time) overhead of creating a perfect hash function
* can we use something about presentation of the vscfmt data to accomplish this?
    * the only time a collision is a problem is when an inserted/changed line
has the same hash (and same length) as the deleted line, within the same region
    * obviously, all of this has an incredibly low probability of occurring, but
if it does....wait
    * ok, what if in the case of collisions (which occur if lines are the exact
same, as well) we just take the line with the same hash in the newer file?
    * ok upon a lengthy cursory analysis i am 99.99% sure this will cover all
cases
    * it won't obviously always catch everything resulting in whole lines being
unnecessarily copied but it will produce CORRECT output
* actual hashing is gonna be djb2 + length of line
    * note that while a gmp bignum is used to keep track of the current bytes
copied, we do NOT need to use one for the length of the line for this hash
    * because even if it rolls over, the number modulo 2^32 is still a
sufficient heuristic for our purposes
*/

/*
 * have fixed LINE_BLOCK size to work through
 * check if hash on first line of right side of block equals hashes on any of
 left side of block
 * if not, iterate through all and check for similarity w/ levenshtein; if
 similar, do by-character diff
 * if a fixed number of blocks (probably just one???) have gone through
        where NO line on right has equaled a hash on the left:
        * then a large number of completely new lines have been introduced, or
 many lines deleted,
        and the cmp process is stopped (force-completed)
 */

#define LEVENSHTEIN_CHECK_CHARS 80
// chosen arbitrarily

typedef struct {
    unsigned long str_hash;   // used because canonical djb2 uses unsigned long
    unsigned long str_length; // arbitrary choice of width
    string_with_size * first_k_chars;
} string_id;
// we do not need to use a bignum for str_length
// even if the string length rolls over, the length modulo 2^32
// is still a sufficient heuristic for uniqueness along with the hash
inline string_id * string_id_set_str_hash(string_id * sid,
                                          unsigned long str_hash) {
    sid->str_hash = str_hash;
    return sid;
}
inline string_id * string_id_set_str_length(string_id * sid,
                                            unsigned long str_length) {
    sid->str_length = str_length;
    return sid;
}
// OPTIMIZATION: take pointers as arguments instead of straight unsigned longs
// and string_with_size
inline string_id * string_id_set_first_k_chars(string_id * sid,
                                               string_with_size * str_k_chars) {
    sid->first_k_chars = str_k_chars;
    return sid;
}
inline string_id * make_string_id(unsigned long str_hash,
                                  unsigned long str_length) {
    return string_id_set_first_k_chars(
      string_id_set_str_length(
        string_id_set_str_hash(malloc(sizeof(string_id)), str_hash),
        str_length),
      set_string_with_size_readable_bytes(
        make_new_string_with_size(LEVENSHTEIN_CHECK_CHARS),
        LEVENSHTEIN_CHECK_CHARS));
}
inline void free_string_id(string_id * sid) {
    free_string_with_size(sid->first_k_chars);
    free(sid);
}
inline bool string_id_equal(string_id * a, string_id * b) {
    return a->str_hash == b->str_hash && a->str_length == b->str_length;
}

// modify LINES_ABOVE_BELOW_TO_SEARCH, not QUEUE_HASH_CRITICAL_SIZE which relies
// on it
#define LINES_ABOVE_BELOW_TO_SEARCH 5
// lines to search above and below active lines for same hashes
#define QUEUE_HASH_CRITICAL_SIZE 2 * LINES_ABOVE_BELOW_TO_SEARCH + 1
// size of line block

// not inlined since is_string_id_in_prev_file takes its address, which
// disallows inlining
void set_bool_if_string_id_match(string_id * prev_string_id,
                                 boolean_and_data * bool_data_bundle);

inline bool is_string_id_in_prev_queue(GQueue * prev_file_queue,
                                       GQueue * cur_file_queue) {
    bool is_string_id_found = false;
    boolean_and_data bool_data_bundle;
    bool_data_bundle.data = g_queue_peek_head(cur_file_queue);
    bool_data_bundle.boolean = &is_string_id_found;
    g_queue_foreach(
      prev_file_queue, (GFunc) set_bool_if_string_id_match, &bool_data_bundle);
    return is_string_id_found;
}

compare_two_result_bytes_processed vcscmp(char * prev_filename,
                                          char * cur_filename);

#endif /*___VCS_CMP_H___*/
