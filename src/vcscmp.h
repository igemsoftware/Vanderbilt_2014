#ifndef ___VCS_CMP_H___
#define ___VCS_CMP_H___

/*
	functions and utilities for implementing character-by-character
	comparison on .vcsfmt-formatted files
*/

// this file is where the money is

#include "utilities.h"

// go through both files, producing list of hashes for each line (again, block I/O)

/*
essentially:
* want the COMPLETE lack of collisions that a perfect hash function has
	* this is critical to determining whether a line is inserted/deleted
* without the (linear time) overhead of creating a perfect hash function
* can we use something about presentation of the vscfmt data to accomplish this?
	* the only time a collision is a problem is when an inserted/changed line has the same hash (and same length) as the deleted line, within the same region
	* obviously, all of this has an incredibly low probability of occurring, but if it does....wait
	* ok, what if in the case of collisions (which occur if lines are the exact same, as well) we just take the line with the same hash in the newer file?
	* ok upon a lengthy cursory analysis i am 99.99% sure this will cover all cases
	* it won't obviously always catch everything resulting in whole lines being unnecessarily copied but it will produce CORRECT output
* actual hashing is gonna be djb2 + length of line
	* note that while a gmp bignum is used to keep track of the current bytes copied, we do NOT need to use one for the length of the line for this hash
	* because even if it rolls over, the number modulo 2^32 is still a sufficient heuristic for our purposes
*/

/*
 * have fixed LINE_BLOCK size to work through
 * check if hash on first line of right side of block equals hashes on any of left side of block
 * if not, iterate through all and check for similarity w/ levenshtein; if similar, do by-character diff
 * if a fixed number of blocks have gone through where NO line on right has equaled a hash on the left:
 		* then a large number of completely new lines have been introduced, or many lines deleted,
		and the cmp process is stopped (force-completed)
 */

typedef struct{
	mpz_t line_number;
	unsigned long str_hash;				// used because canonical djb2 uses unsigned long
	unsigned long str_length;			// arbitrary choice of width
} string_id;
// we do not need to use a bignum for str_length
// even if the string length rolls over, the length modulo 2^32
// is still a sufficient heuristic for uniqueness along with the hash


// compare_two_result_bytes_processed vcscmp (char * prev_filename,
// 																					 char * cur_filename);

#endif /*___VCS_CMP_H___*/
