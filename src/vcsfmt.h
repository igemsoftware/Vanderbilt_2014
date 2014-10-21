#ifndef ___VCS_FMT_H___
#define ___VCS_FMT_H___

/*
    functions for breaking down DNA sequences into specially formatted files
    which work well with traditional line-based version control software
*/
#include "fasta.h"

/* cldoc:begin-category(vcsfmt) */

/* Produces formatted file with extension ".vcsfmt".
 * @filename the file to produce a vcsfmt file from.
 *
 * vcsfmt file produced in same directory as given file.
 * This method simply forwards to the appropriate strategy depending
 * on the original format of the file (FASTA, Genebank, etc.)
 */
void vcsfmt(char * filename);
/* Produces unformatted file from vcsfmt file.
 * @filename the file to produce an unformatted version of.
 *
 * Unformatted file produced in same directory as vcsfmt file.
 * This method simply forwards to the appropriate strategy depending
 * on the original format of the file (FASTA, Genebank, etc.)
 */
void de_vcsfmt(char * filename);

/* cldoc:end-category() */

#endif /*___VCS_FMT_H___*/
