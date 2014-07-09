// provides line-delimiter heuristics for use by vcsfmt

// sources (use actual papers at some point):
// https://en.wikipedia.org/wiki/Start_codon
// https://en.wikipedia.org/wiki/Stop_codon

// TODO: add base-to-amino table
// TODO: change compiler directives to program inputs as functionality expands

#define CODON_LENGTH 3					// codon length, in bases

#if defined ECOLI								// E. coli-only

#if defined POPULAR_CODONS

#define NUMBER_OF_START_CODONS 3
char start_codons[NUMBER_OF_START_CODONS][CODON_LENGTH] =
	// fill with standard codons
	{
		{'A','U','G'},							// AUG (83% prevalence)
		{'G','U','G'},							// GUG (14% prevalence)
		{'U','U','G'}								// UUG (3% prevalence)
	};

#else

#define NUMBER_OF_START_CODONS 5
char start_codons[NUMBER_OF_START_CODONS][CODON_LENGTH] =
	// fill with standard codons
	{
		{'A','U','G'},							// AUG (83% prevalence)
		{'G','U','G'},							// GUG (14% prevalence)
		{'U','U','G'},							// UUG (3% prevalence)
		{'A','U','U'},							// AUU (very small)
		{'C','U','G'}								// CUG (very small)
	};

#endif

#endif

#if defined EUKARYOTE

#define NUMBER_OF_START_CODONS = 1
char start_codons[NUMBER_OF_START_CODONS][CODON_LENGTH] = { { 'A', 'U', 'G'  } };
// eukaryotes rarely use anything other than AUG

#endif
