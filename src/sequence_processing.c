#include "sequence_processing.h" // required

// sources (use actual papers at some point):
// https://en.wikipedia.org/wiki/Start_codon
// https://en.wikipedia.org/wiki/Stop_codon

// TODO: change compiler directives to program inputs as functionality expands

#define CODON_LENGTH 3 // codon length, in bases

// start codons

#if defined ECOLI // E. coli-only

#if defined SINGLE_START_CODON
#define NUMBER_OF_START_CODONS 1
const char * start_codons[ NUMBER_OF_START_CODONS ] = {"ATG"};
#elif defined POPULAR_CODONS
#define NUMBER_OF_START_CODONS 3
const char * start_codons[ NUMBER_OF_START_CODONS ] = {
    "ATG", // AUG (83% prevalence)
    "GTG", // GUG (14% prevalence)
    "TTG"  // UUG (3% prevalence)
};
#else
#define NUMBER_OF_START_CODONS 5
const char * start_codons[ NUMBER_OF_START_CODONS ] = {
    "ATG", // AUG (83% prevalence)
    "GTG", // GUG (14% prevalence)
    "TTG", // UUG (3% prevalence)
    "ATT", // AUU (very small)
    "CTG"  // CUG (very small)
};
#endif

#endif

#if defined EUKARYOTE
#define NUMBER_OF_START_CODONS 1
const char * start_codons[ NUMBER_OF_START_CODONS ] = {"ATG"};
// eukaryotes rarely use anything other than ATG
#endif

// stop codons
#define NUMBER_OF_STOP_CODONS 3
const char * stop_codons[ NUMBER_OF_STOP_CODONS ] = {"TAA", "TAG", "TGA"};

bool is_start_codon(char * codon) {
  for (unsigned long long start_codon_index = 0;
       start_codon_index < NUMBER_OF_START_CODONS; ++start_codon_index) {
    if (strncmp(codon, start_codons[ start_codon_index ], CODON_LENGTH) == 0) {
      return true;
    }
  }
  return false;
}

bool is_stop_codon(char * codon) {
  for (unsigned long long stop_codon_index = 0;
       stop_codon_index < NUMBER_OF_STOP_CODONS; ++stop_codon_index) {
    if (strncmp(codon, stop_codons[ stop_codon_index ], CODON_LENGTH) == 0) {
      return true;
    }
  }
  return false;
}
