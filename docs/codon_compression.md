* can't unambiguously differentiate newline characters if we assign A = 00, C = 01, G = 10, T = 11, since newline '\n' is 00001010
* so, assign bit strings to codons; since there exist 64 codon (2^6), then we can represent each codon with 6 bits. Make the first two '11' for codons, and '00' for delimiters.
* We can use these last two bits to differentiate codons from \n, and maintain the delimiters we're looking for.
