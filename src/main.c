#include <stdio.h>              // for printf

#include "vcsfmt.h"             // for file processing and I/O

int main(){
  int result_of_vcsfmt = vcsfmt("500_lines_of_dna_minus_lines.fasta");
  if (result_of_vcsfmt == -1){
    PRINT_ERROR("vcsfmt failed.\n");
  }
  else{
    printf("Succeeded!\n");
  }

  int result_of_de_vcsfmt = de_vcsfmt("500_lines_of_dna_minus_lines.fasta.vcsfmt");
  if (result_of_de_vcsfmt == -1){
    PRINT_ERROR("de_vcsfmt failed.\n");
  }
  else{
    printf("Succeeded!\n");
  }
}
